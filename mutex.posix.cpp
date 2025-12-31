/*
* @file        mutex.posix.cpp
* @brief       互斥量,用于对可能被多个线程同时访问的资源加锁
* @author      sunu<pansunyou@gmail.com>
*
* @copyright   xgd
*/
#include <base/mutex.hpp>
#include <base/charset.hpp>
#include <base/toolkit.hpp>
//#include <logger.hpp>

#if !defined(Z_OS_WIN32)

#include <errno.h>
#include <pthread.h>
#include <cstring>
#include <cstdio>
#include <sys/time.h>

#define ___DBG fprintf(stderr, "%p > %s, %s:%03d\n", this, __FILE__, __FUNCTION__, __LINE__);
#undef ___DBG
#define ___DBG

#ifdef Z_HAVE_SYSV_SHAREDMEM
    #include <sys/types.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>
#endif

using namespace aps;

struct Mutex::mutex_handle_t
{
    pthread_mutex_t  mutex;
    int              shmid;
    unsigned int     refcount;
};

Mutex::Mutex(const char* name /*=0*/) noexcept(false)
{
    ___DBG;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);

    if(name)
    {
#if defined(Z_HAVE_SYSV_SHAREDMEM) && defined(Z_HAVE_PTHREAD_MUTEXATTR_SETPSHARED)
        key_t shmkey = ftok(name, 0);

        int shmid = shmget(shmkey, sizeof(mutex_handle_t), IPC_CREAT);
        int r = errno;
        if(shmid == -1)
            throw Exception(Z_SOURCEINFO, r, "shmget fail");

        mHandle = (mutex_handle_t*)shmat(shmid,0,0);
        mHandle->shmid    = shmid;
        mHandle->refcount = 1;
        pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
#else
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "当前平台无法使用跨进程的命名互斥量.");
#endif
    }
    else
    {
        mHandle = new mutex_handle_t;
        mHandle->shmid    = -1;
        mHandle->refcount = 1;

#ifdef Z_HAVE_PTHREAD_MUTEXATTR_SETPSHARED
        pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_PRIVATE);
#endif
    }

    pthread_mutex_init(&mHandle->mutex,&attr);
}

Mutex::~Mutex()
{
    ___DBG;
    
    if((--mHandle->refcount) == 0)
        pthread_mutex_destroy(&mHandle->mutex);

#ifdef Z_HAVE_SYSV_SHAREDMEM
    if(mHandle->shmid != -1)
    {
        int shmid = mHandle->shmid;

        if(!mHandle->refcount)
            shmctl(shmid,IPC_RMID,0);

        shmdt((void*)mHandle);
    }
    else
#endif
    delete mHandle;
}

void Mutex::lock() noexcept(false)
{
    ___DBG;
    int ret = pthread_mutex_lock(&mHandle->mutex);
    if(ret != 0)
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
    }
}

void Mutex::unlock() noexcept(false)
{
    ___DBG;
    int ret = pthread_mutex_unlock(&mHandle->mutex);
    if(ret != 0)
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
    }
}

#endif

