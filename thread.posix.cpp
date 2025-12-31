/*
 * @file        thread.cpp
 * @brief       线程简单封装
 * @author      leize<leize@xgd.com>、sunu<pansunyou@gmail.com>
 * @copyright   xgd
*/
#include <thread.hpp>
#include <toolkit.hpp>

#ifdef Z_OS_LINUX
#include <base/event.hpp>
#include <base/datetime.hpp>
#include <base/charset.hpp>
#include <base/logger.hpp>

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <cstring>
#include <pthread.h>

#ifndef AIX
#include <sys/syscall.h>
#endif

using namespace aps;

//
// Block SIGPIPE in main thread.
//

class SignalBlocker
{
public:
    SignalBlocker()
    {
        sigset_t sset;
        sigemptyset(&sset);
        sigaddset(&sset, SIGPIPE); 
        pthread_sigmask(SIG_BLOCK, &sset, 0);
    }
    ~SignalBlocker()
    {
    }
};

static SignalBlocker signalBlocker;

struct Thread::ThreadPrivate_t
{
    struct CallbackData
    {
        CallbackData(): callback(0), pData(0)
        {}
        Callable_t  callback;
        void*     pData; 
    };

    ThreadPrivate_t()
    {
        pRunnableTarget_ = 0;
        stackSize_ = 0;
    }

    Runnable*   pRunnableTarget_;
    CallbackData callbackTarget;
    pthread_t     thread_;
    int           prio_;
    int           osPrio_;
    Event         done_;
    std::size_t   stackSize_;
};


Thread::Thread():
mIsDeleteThisAfter(false),
mIsRunning(false),
mIsTerminated(false),
mIsDone(false),
mUniqueId(createUniqueId()),
mThreadName(makeName()),
mThreadId(0),
mPrivate(new ThreadPrivate_t)
{
    Z_LOG_X(eTRACE) << "CREATE";
}


Thread::Thread(const string name): 
mIsDeleteThisAfter(false),
mIsRunning(false),
mIsTerminated(false),
mIsDone(false),
mUniqueId(createUniqueId()),
mThreadName(name), 
mThreadId(0),
mPrivate(new ThreadPrivate_t)
{
    Z_LOG_X(eTRACE) << "CREATE";
}


Thread::~Thread()
{
}


void Thread::sleep(long millseconds)
{
    TUInt64 remainingTime = millseconds*1000; //微秒
    int rc;
    do
    {
        struct timespec ts;
        ts.tv_sec  = (long) remainingTime / 1000000L ;
        ts.tv_nsec = (long) (remainingTime % 1000000L) * 1000 ; // 纳秒
        Time start = Time::now();
        rc = ::nanosleep(&ts, 0);
        if (rc < 0 && errno == EINTR)
        {
            TUInt64 waited = Time::now().getMillSecond() - start.getMillSecond();
            if (waited < remainingTime)
                remainingTime -= waited;
            else
                remainingTime = 0;
        }
    }
    while (remainingTime > 0 && rc < 0 && errno == EINTR);
    if (rc < 0 && remainingTime > 0) 
        throw Exception(Z_SOURCEINFO, -1, ("Thread::sleep(): nanosleep()失败"));
}

void Thread::yield()
{
#ifndef AIX
    sched_yield();
#endif
}

void Thread::start(Runnable* target, bool bSuspended)
{
    Z_LOG_X(eTRACE) << "START";
    if (mIsRunning)
        throw Exception(Z_SOURCEINFO, -1, ("此线程已经在运行."));

    if (mIsDone)
        throw Exception(Z_SOURCEINFO, -1, ("Thread对象不能复用."));

    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_JOINABLE);

#ifdef _POSIX_THREAD_ATTR_STACKSIZE
    pthread_attr_setstacksize(&attributes, 0);
#endif

    mPrivate->pRunnableTarget_ = target;
    if (pthread_create(&mPrivate->thread_, &attributes, runnableEntry, this))
    {
        int r = errno;
        mPrivate->pRunnableTarget_ = 0;
        pthread_attr_destroy(&attributes);
        throw Exception(Z_SOURCEINFO, r, ("无法创建线程"));
    }
    pthread_attr_destroy(&attributes);
}


void Thread::start(Callable_t target, void* pData, bool bSuspended)
{
    Z_LOG_X(eTRACE) << "START";
    Z_ASSERT (NULL!=target);

    if (mIsRunning)
        throw Exception(Z_SOURCEINFO, -1, ("此线程已经在运行."));

    if (mIsDone)
        throw Exception(Z_SOURCEINFO, -1, ("Thread对象不能复用."));

    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_JOINABLE);
#ifdef _POSIX_THREAD_ATTR_STACKSIZE
    pthread_attr_setstacksize(&attributes, 0);
#endif

    mPrivate->callbackTarget.callback = target;
    mPrivate->callbackTarget.pData = pData;

    if (pthread_create(&mPrivate->thread_, &attributes, callableEntry, this))
    {
        int r = errno;
        mPrivate->callbackTarget.callback = 0;
        mPrivate->callbackTarget.pData = 0;
        pthread_attr_destroy(&attributes);
        throw Exception(Z_SOURCEINFO, r, ("无法创建线程"));
    }
    pthread_attr_destroy(&attributes);

}


void Thread::wait()
{
    Z_LOG_X(eTRACE) << "wait";
    if (mIsDeleteThisAfter==true)
        return ;
    mPrivate->done_.wait();
    void* result;
    if (pthread_join(mPrivate->thread_, &result))
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, strerror(r));
    }
}


//! 返回当前线程(主线程调用时返回0)
Thread* Thread::getCurrentThread()
{
    return Thread::mCurThreadHolder;
}


//! WIN32平台同currentTid
int Thread::getCurrentThreadId()
{
#ifdef AIX
    return (int)pthread_self();
#else   
    return (int)syscall(SYS_gettid);
#endif
}


//! 用于执行CRuncable子类
void * Thread::runnableEntry(void* _pThread)
{
    Thread* pThread = reinterpret_cast<Thread*>(_pThread);
    Thread::mCurThreadHolder = pThread;

    {
        Mutex::ScopedLock l(pThread->mLock);
        pThread->mThreadId = Thread::getCurrentThreadId();
        pThread->mIsRunning = true;
    }

    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGQUIT);
    sigaddset(&sset, SIGTERM);
    sigaddset(&sset, SIGPIPE); 
    pthread_sigmask(SIG_BLOCK, &sset, 0);

    try
    {
        pThread->mPrivate->pRunnableTarget_->run();
        
        {
            Mutex::ScopedLock l(pThread->mLock);
            pThread->mIsRunning = false;
            pThread->mIsDone = true;
            pThread->mPrivate->pRunnableTarget_ = NULL;
            pThread->mPrivate->done_.set();
        }

        if (pThread->mIsDeleteThisAfter)
        {
            pthread_detach(pthread_self());
            delete pThread;
        }
    }
    catch (Exception& e)
    {
        Exception::defaultExceptionProcesser(e);
    }
    catch (std::exception& e)
    {
        Exception ee(Z_SOURCEINFO, -1, e.what());
        Exception::defaultExceptionProcesser(ee);
    }
    catch (...)
    {
        Exception e(Z_SOURCEINFO, -1, ("捕获未知异常"));
        Exception::defaultExceptionProcesser(e);
    }

    return 0;
}


//! 用于执行静态函数
void * Thread::callableEntry(void* _pThread)
{
    Thread* pThread = reinterpret_cast<Thread*>(_pThread);
    Thread::mCurThreadHolder = pThread;
    
    {
        Mutex::ScopedLock l(pThread->mLock);
        pThread->mThreadId = Thread::getCurrentThreadId();
        pThread->mIsRunning = true;
    }

    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGQUIT);
    sigaddset(&sset, SIGTERM);
    sigaddset(&sset, SIGPIPE); 
    pthread_sigmask(SIG_BLOCK, &sset, 0);

    try
    {
        pThread->mPrivate->callbackTarget.callback(pThread->mPrivate->callbackTarget.pData);

        {
            Mutex::ScopedLock l(pThread->mLock);
            pThread->mIsRunning = false;
            pThread->mIsDone = true;
            pThread->mPrivate->callbackTarget.callback = NULL;
            pThread->mPrivate->callbackTarget.pData = NULL;
            pThread->mPrivate->done_.set();
        }

        if (pThread->mIsDeleteThisAfter)
        {
            pthread_detach(pthread_self());
            delete pThread;
        }

    }
    catch (Exception& e)
    {
        Exception::defaultExceptionProcesser(e);
    }
    catch (std::exception& e)
    {
        Exception ee(Z_SOURCEINFO, -1, e.what());
        Exception::defaultExceptionProcesser(ee);
    }
    catch (...)
    {
        Exception e(Z_SOURCEINFO, -1, ("捕获未知异常"));
        Exception::defaultExceptionProcesser(e);
    }

    return 0;
}

#endif
