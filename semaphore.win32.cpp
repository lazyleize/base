/*
* @file        semaphore.cpp
* @brief       信号量
*  一般应用: 多进程使用同一资源池
* @author      leize<leize@xgd.com>
* @copyright   xgd
*/
#include <base/semaphore.hpp>
#include <base/platform.hpp>
#include <base/charset.hpp>

#ifdef Z_OS_WIN32

using namespace aps;

#define MAX_SEM_VALUE  LONG_MAX

struct Semaphore::sem_handle_t 
{
    HANDLE handle;
};

Semaphore::Semaphore(unsigned int initial,const char* name)throw(Exception)
{
	HANDLE handle = CreateSemaphore(NULL,initial,MAX_SEM_VALUE,name);
	if(!handle)
		throw Exception(Z_SOURCEINFO,GetLastError(),("CreateSemaphore调用失败."));

	mHandle = new sem_handle_t;
	mHandle->handle = handle;
}

Semaphore::~Semaphore()throw(Exception)
{
	CloseHandle(mHandle->handle);
	delete mHandle;
}

void Semaphore::wait()throw(Exception)
{
	DWORD ret = WaitForSingleObjectEx(mHandle->handle,INFINITE,FALSE);
	if(ret == WAIT_FAILED)
		throw Exception(Z_SOURCEINFO,GetLastError(),("无法减小信号量."));
}

bool Semaphore::tryWait()throw(Exception)
{
	DWORD ret = WaitForSingleObjectEx(mHandle->handle,0,FALSE);
	if(ret == WAIT_FAILED)
		throw Exception(Z_SOURCEINFO,GetLastError(),("无法减小信号量."));
	else if(ret == WAIT_OBJECT_0)
		return true;
	return false;
}

void Semaphore::post() throw(Exception)
{
    if(!ReleaseSemaphore(mHandle->handle, 1, NULL))
        throw Exception(Z_SOURCEINFO, GetLastError(), ("无法增加信号量."));
}

#endif