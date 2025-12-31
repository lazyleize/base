/*
* @file        thread.cpp
* @brief       线程简单封装
* @author      leize、sunu<pansunyou@gmail.com>
* @copyright   xgd
*/
#include <base/thread.hpp>
#include <base/platform.hpp>
#include <base/event.hpp>
#include <base/charset.hpp>
#include <base/logger.hpp>

#include <sstream>
#include <ctime>
#include <list>
#include <stack>
#include <algorithm>

using namespace aps;

using namespace std;

ThreadKey<Thread> Thread::mCurThreadHolder;

string Thread::makeName()
{
    ostringstream name;
    name << '#' << mUniqueId;
    return name.str();
}

void Thread::deleteAfter(bool bSetDelAfter)
{
    Mutex::ScopedLock l(mLock);
    mIsDeleteThisAfter = bSetDelAfter;
}


string Thread::name()
{
    Mutex::ScopedLock l(mLock);
    return mThreadName;
}


int Thread::uniqueId()
{
    Mutex::ScopedLock l(mLock);
    return mUniqueId;
}

void Thread::stop()
{
    Mutex::ScopedLock l(mLock);
    mIsTerminated = true;
}


//! 线程还在工作
bool Thread::isRunning()
{
    Mutex::ScopedLock lo(mLock);
    return mIsRunning;
}

namespace
{
    static Mutex uniqueIdMutex;
}


int Thread::createUniqueId()
{
    Mutex::ScopedLock lock(uniqueIdMutex);
    static unsigned count = 0;
    ++count;
    return count;
}


//////////////////////////////////////////////////////////////////////////

namespace aps
{
    //! 线程池中的工作线程类
    class LIBAPS_API PooledThread : public Thread
    {
    public:
        PooledThread(ThreadPool& pool);

        //! 直到线程结束才返回
        void stop();

        //! 通过这个接口重新分配任务
        void runTask(Runnable* target);

        ThreadPool& getThreadPool();

    protected:

        // 等待任务被分配, 完成后原地待命.
        static void runTaskWait( void *pThis );

    private:
        bool m_bTaskRunning;
        Event m_bContinue;

        Runnable* pRunnableTarget;

        ThreadPool& m_ownToPool;
        friend class ThreadPool;
    };


    PooledThread::PooledThread(ThreadPool& pool):
    m_bTaskRunning(false),
        m_ownToPool(pool)
    {
        start(runTaskWait, this);
    }


    //! 直到线程结束才返回
    void PooledThread::stop()
    {
        Mutex::ScopedLock lo(mLock);
        mIsTerminated = true;
        m_bContinue.set();
    }


    //! 通过这个接口重新分配任务
    void PooledThread::runTask(Runnable* target)
    { 
        Z_ASSERT( m_bTaskRunning == false );
        Mutex::ScopedLock lo(mLock);
        pRunnableTarget = target;
        m_bContinue.set();
    }


    // 等待任务被分配, 完成后原地待命.
    void PooledThread::runTaskWait( void *pThis )
    {
        PooledThread *pThread = (PooledThread*)pThis;
        Z_ASSERT( pThread!=NULL );


        bool saveThread = false;
        int bRunTime = 0;
        //CCriticalSection::ScopedLock lo(pThread->m_lock);
        for (; (bRunTime++) <=0 || !pThread->mIsTerminated;)
        {
            pThread->m_bContinue.wait();
            if (!pThread->mIsTerminated)
            {
                if (pThread->pRunnableTarget!=NULL)
                {
                    pThread->m_bTaskRunning = true;
                    pThread->pRunnableTarget->run();
                    pThread->m_bTaskRunning = false;
                    delete pThread->pRunnableTarget;
                    pThread->pRunnableTarget = NULL;
                }
            }

            pThread->m_bTaskRunning = false;
            pThread->pRunnableTarget = NULL;

            // 返回false表示这个线程不再使用.
            saveThread = pThread->m_ownToPool.saveThreadToIdel(pThread);
            if (!saveThread)
                break;
        }

        // 这个线程要被抛弃了.
        {
            Mutex::ScopedLock scope(pThread->m_ownToPool.mPoolMutex);
            std::stack<PooledThread*> tmpPool;
            for (;pThread->m_ownToPool.mIdleTheads.size()>0;)
            {
                PooledThread* pThread_ = pThread->m_ownToPool.mIdleTheads.top();
                pThread->m_ownToPool.mIdleTheads.pop();
                tmpPool.push(pThread_);
            }
            for (;tmpPool.size()>0;)
            {
                PooledThread* pThread_ = tmpPool.top();
                tmpPool.pop();
                if (pThread_!=pThread)
                    pThread->m_ownToPool.mIdleTheads.push(pThread_);
            }

            if ( (pThread->m_ownToPool.mIdleTheads.size() + pThread->m_ownToPool.mActiveTheads.size()) <=0 )
            {
                pThread->m_ownToPool.mPoolEmptyCond.notify();
            }
        }
        pThread->deleteAfter(true);
    }

    ThreadPool& PooledThread::getThreadPool()
    {
        return m_ownToPool;
    }
}

//////////////////////////////////////////////////////////////////////////

namespace 
{

    class CRunnableObj : public Runnable 
    {
    public:
        CRunnableObj(Callable_t target, void* pArgData)
        {
            setParam(target, pArgData);
        }

        void setParam(Callable_t target, void* pArgData)
        {
            target_ =target;
            pArgData_ =pArgData;
        }

        void run()
        {
            Z_ASSERT(target_!=NULL);
            target_(pArgData_);
        }

        Callable_t target_;
        void* pArgData_;
    };

}

ThreadPool::ThreadPool(int maxThreadCount, string poolName, int initThread/*=0*/):
mHaveIdleCond(mPoolMutex),
mActiveEmptyCond(mPoolMutex),
mPoolEmptyCond(mPoolMutex),
mMaxThreadCount(maxThreadCount),
mPoolName(poolName),
mIsShutdown(false)
{
    if (mMaxThreadCount<=0)
        mMaxThreadCount = 1;

    if (initThread>mMaxThreadCount)
        initThread = mMaxThreadCount;

    //立即创建新线程
    if (initThread>0)
        ceateThreads(initThread);
}


void ThreadPool::ceateThreads( int n )
{
    Mutex::ScopedLock scope(mPoolMutex);

    int r = n>0?n:0;
    if ( (int)(r+mActiveTheads.size()+mIdleTheads.size())> mMaxThreadCount )
        r = mMaxThreadCount - ( mActiveTheads.size()+mIdleTheads.size() );

    for (int i=0; i<r ;i++)
    {
        PooledThread* pThread = new PooledThread(*this);
        mActiveTheads.push_back(pThread);
        pThread->runTask(new CRunnableObj(nullThreadFunc, NULL));
    }
}



ThreadPool::~ThreadPool()
{
}


void ThreadPool::setCapatity(int newSize)
{
    Mutex::ScopedLock scope(mPoolMutex);
    mMaxThreadCount = newSize;
}


bool ThreadPool::dispatch(Runnable* target, bool bWait)
{
    PooledThread * pThread = NULL;
    Mutex::ScopedLock scope(mPoolMutex);

    if (mIsShutdown)
    {
        delete target;
        return false;
    }

    //等到有空闲的线程
    for( ; mIdleTheads.size() <= 0 && (int)(mActiveTheads.size()+mIdleTheads.size()) >= mMaxThreadCount; )
    {
        if (bWait)
            mHaveIdleCond.wait();
        else
        {
            delete target;
            return false;
        }

        if (mIsShutdown)
        {
            delete target;
            return false;
        }
    }

    // 没有空闲线程, 并且可以创建更多线程
    if( mIdleTheads.size() <= 0 ) 
    {
        try
        {
            pThread = new PooledThread(*this);
            mActiveTheads.push_back(pThread);
            pThread->runTask(target);
        }
        catch(...)
        {
            if (pThread)
                delete pThread;
            delete target;
            return false;
        }
    } 
    else
    {
        // 取用一个线程, 放入active队列
        pThread = mIdleTheads.top();
        mIdleTheads.pop();
        mActiveTheads.push_back(pThread);

        // 通知这个线程继续干活
        pThread->runTask(target);
    }

    return true;
}


bool ThreadPool::dispatch(Callable_t target, void* pArgData, bool bWait)
{
    return dispatch(new CRunnableObj(target, pArgData), bWait);
}


void ThreadPool::nullThreadFunc( void* )
{
}


void ThreadPool::freeThread(PooledThread*pThread)
{
}


//! 闲置的线程放到队列里, 后进先出
bool ThreadPool::saveThreadToIdel( PooledThread * pThread)
{
    Z_ASSERT(pThread!=NULL);
    Mutex::ScopedLock scope(mPoolMutex);

    // 从acitve里移除
    std::list<PooledThread *>::iterator it = find(mActiveTheads.begin(), mActiveTheads.end(), pThread);
    if (it!=mActiveTheads.end())
        mActiveTheads.erase(it);

    bool r = false;
    if ( !mIsShutdown && (int)( mIdleTheads.size() + mActiveTheads.size() ) < mMaxThreadCount)
    {
        // 加入idle队列
        mIdleTheads.push(pThread);
        mHaveIdleCond.notify();
        r = true;
    }

    // 已无活动线程
    if (mActiveTheads.size()<=0)
        mActiveEmptyCond.notify();

    return r;
}


string ThreadPool::name()
{
    Mutex::ScopedLock scope(mPoolMutex);
    return mPoolName;
}



void ThreadPool::stop()
{
    Mutex::ScopedLock scope(mPoolMutex);
    Z_LOG_X(eTRACE) << "Thread Pool (" << mPoolName << ") is stopping,  " << mActiveTheads.size() << " ACTIVE threads exists ... ";
    mIsShutdown = true;

    std::list<PooledThread*>::const_iterator it = mActiveTheads.begin();
    for (;it!=mActiveTheads.end();it++)
    {
        PooledThread*pThread  = *it;
        pThread->stop();
    }

    //激活所有空闲线程, 让其退出
    Z_LOG_X(eTRACE) << mIdleTheads.size() << " IDLE threads exists ... ";
    std::stack<PooledThread*> tmpPool;
    for (;mIdleTheads.size()>0;)
    {
        PooledThread* pThread = mIdleTheads.top();
        mIdleTheads.pop();
        pThread->stop();
        tmpPool.push(pThread);
    }

    for (;tmpPool.size()>0;)
    {
        PooledThread* pThread = tmpPool.top();
        tmpPool.pop();
        mIdleTheads.push(pThread);
    }
}


//! 直到所有线程退出并释放才返回
void ThreadPool::wait()
{
    Mutex::ScopedLock scope(mPoolMutex);
    Z_LOG_X(eTRACE) << "Thread Pool (" << mPoolName << ") waiting " << mActiveTheads.size() << " ACTIVE threads to exit ... ";

    //所有活动线程已结束?
    for (;mActiveTheads.size() >0;)
    { 
        //在进入wait后, 上面的lock会被暂时性释放
        mActiveEmptyCond.wait(); 
    }

    // 等待所有idle线程退出(锁在这里暂时释放)
    Z_LOG_X(eTRACE) << "Thread Pool (" << mPoolName << ") waiting " << mIdleTheads.size() << " IDLE threads to exit ... ";
    for (;mIdleTheads.size()>0;)
    {
        mPoolEmptyCond.wait();
    }

    Z_LOG_X(eTRACE) << "Thread Pool (" << mPoolName << ") has no anymore sub-threads. ";
}


//! 当前多少闲置线程
int ThreadPool::idleCount()
{
    Mutex::ScopedLock scope(mPoolMutex);
    return mIdleTheads.size();
}

//! 当前有多少在执行任务
int ThreadPool::activeCount()
{
    Mutex::ScopedLock scope(mPoolMutex);
    return mActiveTheads.size();
}

//! 最大允许多少个线程
int ThreadPool::capacity()
{
    Mutex::ScopedLock scope(mPoolMutex);
    return mMaxThreadCount;
}
