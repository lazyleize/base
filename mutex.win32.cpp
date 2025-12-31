/*
* @file        mutex.win32.cpp
* @brief       互斥量,用于对可能被多个线程同时访问的资源加锁
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#include <base/mutex.hpp>
#include <base/charset.hpp>
#include <base/toolkit.hpp>

#ifdef Z_OS_WIN32
#include <windows.h>

using namespace aps;

struct Mutex::mutex_handle_t
{
    HANDLE handle;
};

Mutex::Mutex(const char* name /*=0*/) noexcept(false)
{
    HANDLE handle = CreateMutex(NULL, FALSE, name);
    if(!handle)
        throw Exception(Z_SOURCEINFO, GetLastError(), ("CreateMutex调用失败."));

    mHandle = new mutex_handle_t;
    mHandle->handle = handle;
}

Mutex::~Mutex() throw()
{
    Z_ASSERT(mHandle!=NULL);
    CloseHandle(mHandle->handle);
    delete mHandle;
}

void Mutex::lock() noexcept(false)
{
    Z_ASSERT(mHandle!=NULL);
    DWORD ret = WaitForSingleObjectEx(mHandle->handle, INFINITE, FALSE);
    if(ret == WAIT_FAILED)
        throw Exception(Z_SOURCEINFO, GetLastError(), ("WaitForSingleObjectEx调用失败."));
}


void Mutex::unlock() noexcept(false)
{
    Z_ASSERT(mHandle!=NULL);
    if(!ReleaseMutex(mHandle->handle))
        throw Exception(Z_SOURCEINFO, GetLastError(), ("ReleaseMutex调用失败."));
}


#endif

