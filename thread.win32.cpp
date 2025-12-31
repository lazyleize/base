/*
 * @file        thread.cpp
 * @brief       线程简单封装
 * @author      leize、sunu<pansunyou@gmail.com>
 * @copyright   xgd
*/
#include <base/thread.hpp>
#include <base/platform.hpp>
#include <base/exception.hpp>
#include <base/charset.hpp>
#include <base/toolkit.hpp>

#ifdef Z_OS_WIN32

#include <Windows.h>
#include <process.h>

using namespace aps;

struct Thread::ThreadPrivate_t
{
	struct CallbackData
	{
		CallbackData()
		{
			callback = 0;
			pData = 0;
		}

		Callable_t  callback;
        void*     pData; 
	};

	ThreadPrivate_t()
	{
		_pRunnableTarget = 0;
        _thread = INVALID_HANDLE_VALUE;
        _threadId = 0;
        _prio = 0;
        _stackSize = 0;
	}

	Runnable*   _pRunnableTarget;
    CallbackData _callbackTarget;
    HANDLE       _thread;
    DWORD        _threadId;
    int          _prio;
    int          _stackSize;
};

Thread::Thread():
mIsSuspended(false),
mIsDeleteThisAfter(false),
mIsRunning(false),
mIsTerminated(false),
mIsDone(false),
mUniqueId(createUniqueId()),
mThreadName(makeName()),
mThreadId(0),
mPrivate(new ThreadPrivate_t)
{
}

Thread::Thread(const string name): 
mIsSuspended(false),
mIsDeleteThisAfter(false),
mIsRunning(false),
mIsTerminated(false),
mIsDone(false),
mUniqueId(createUniqueId()),
mThreadName(name), 
mThreadId(0),
mPrivate(new ThreadPrivate_t)
{
}

Thread::~Thread()
{
}

void Thread::sleep(long millseconds)
{
    Sleep(DWORD(millseconds));
}

void Thread::yield()
{
    Sleep(0);
}

//! 等待进程结束
void Thread::wait()
{
	if(mPrivate->_thread == INVALID_HANDLE_VALUE || mIsDeleteThisAfter == true)
		return ;

	DWORD rr = WaitForSingleObject(mPrivate->_thread,INFINITE);
	switch(rr)
	{
	case WAIT_OBJECT_0:
		if(CloseHandle(mPrivate->_thread))
			mPrivate->_thread = INVALID_HANDLE_VALUE;
		return ;
	default:
		{
			int r = GetLastError();
			throw Exception(Z_SOURCEINFO,r,Toolkit::formatError(r));
		}
	}
}

//! 返回当前线程(主线程调用时返回0)
Thread* Thread::getCurrentThread()
{
    return Thread::mCurThreadHolder.get();
}

//! WIN32平台同currentTid
int Thread::getCurrentThreadId()
{
    return GetCurrentThreadId();
}

//! 在线程中运行可执行对象
void Thread::start(Runnable* target, bool bSuspended)
{
	if (mIsRunning)
        throw Exception(Z_SOURCEINFO, -1, ("此线程已经在运行."));

    if (mIsDone)
        throw Exception(Z_SOURCEINFO, -1, ("Thread对象不能复用."));

    mIsSuspended = bSuspended;
    mPrivate->_pRunnableTarget = target;
    create(runnableEntry, this);
}

//! 在进程中运行函数(回调)
void Thread::start(Callable_t target, void* pData, bool bSuspended)
{
    Z_ASSERT (NULL!=target);

    if (mIsRunning)
        throw Exception(Z_SOURCEINFO, -1, ("此线程已经在运行."));

    if (mIsDone)
        throw Exception(Z_SOURCEINFO, -1, ("Thread对象不能复用."));

    mIsSuspended = bSuspended;
    mPrivate->_callbackTarget.callback = target;
    mPrivate->_callbackTarget.pData = pData;
    create(callableEntry, this);
}

//! 创建进程
void Thread::create(Entry ent, void* pData)
{
	unsigned int threadId;
	int suspended = (mIsSuspended)?CREATE_SUSPENDED:0;
	mPrivate->_thread = (HANDLE)_beginthreadex(NULL,mPrivate->_stackSize,ent,this, suspended, &threadId);
	int r = GetLastError();
	mPrivate->_threadId = static_cast<DWORD>(threadId);

	if(mPrivate->_thread == INVALID_HANDLE_VALUE)
		throw Exception(Z_SOURCEINFO,r,string("创建线程失败:") + Toolkit::formatError(r));
}

//! 执行暂停的线程
void Thread::resume()
{
	if(!mIsSuspended)
		//throw CException(Z_SOURCEINFO, -1, ("此线程创建时即运行, 不能调用此函数:"));
		return ;

	ResumeThread(mPrivate->_thread);
}

//! 用于执行CRuncable子类
unsigned __stdcall Thread::runnableEntry(void* _pThread)
{
	Thread* pThread = (Thread*)_pThread;
	Thread::mCurThreadHolder.set(pThread);
	{
		Mutex::ScopedLock l(pThread->mLock);
		pThread->mThreadId = Thread::getCurrentThreadId();
		pThread->mIsRunning = true;
	}

	try
	{
		pThread->mPrivate->_pRunnableTarget->run();
		{
			Mutex::ScopedLock l(pThread->mLock);
			pThread->mIsRunning = false;
			pThread->mIsDone = true;
			pThread->mPrivate->_pRunnableTarget = NULL;
		}
		if(pThread->mIsDeleteThisAfter)
		{
			CloseHandle(pThread->mPrivate->_thread);
			delete pThread;
		}
	}
	catch(Exception& e)
	{
		Exception::defaultExceptionProcesser(e);
	}
	catch(std::exception& e)
	{
		Exception ee(Z_SOURCEINFO,-1,e.what());
		Exception::defaultExceptionProcesser(ee);
	}
	catch(...)
	{
		Exception e(Z_SOURCEINFO, -1, ("捕获未知异常"));
        Exception::defaultExceptionProcesser(e);
	}
	return 0;
}

//! 用于执行静态函数
unsigned __stdcall Thread::callableEntry(void* _pThread)
{
	Thread* pThread = (Thread*)_pThread;
    Thread::mCurThreadHolder.set( pThread );
    {
        Mutex::ScopedLock l(pThread->mLock);
        pThread->mThreadId = Thread::getCurrentThreadId();
        pThread->mIsRunning = true;
    }

    try
    {
        pThread->mPrivate->_callbackTarget.callback(pThread->mPrivate->_callbackTarget.pData);
        {
            Mutex::ScopedLock l(pThread->mLock);
            pThread->mIsRunning = false;
            pThread->mIsDone = true;
        }

        if (pThread->mIsDeleteThisAfter)
        {
            CloseHandle(pThread->mPrivate->_thread);
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