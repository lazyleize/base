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
#include <base/exception.hpp>

#ifdef Z_OS_LINUX

#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <cstdlib>
#include <errno.h>

using namespace aps;

struct Semaphore::sem_handle_t 
{
    sem_t* semaphore;
    char* name;
    bool creator;
};

Semaphore::Semaphore(unsigned int initial, const char* name)noexcept(false)
{
    mHandle = new sem_handle_t;

    if(name)
    {
//#ifndef Z_HAVE_SEM_OPEN
//        delete mHandle;
//        throw Exception(Z_SOURCEINFO, -1, "Semaphore not supoort on this platform");
//#endif
        sem_t* sem;
        int flags = O_CREAT|O_EXCL;

_sem_open:
        sem = sem_open(name, flags, 0, initial);
        int r = errno;
        if(sem == SEM_FAILED)
        {
            if(r != EEXIST)
            {
                delete mHandle;
                throw Exception(Z_SOURCEINFO, r, "sem_open fail");
            }

            if(!flags)
                flags = O_CREAT|O_EXCL;
            else
                flags = 0;

            goto _sem_open;
        }

        mHandle->semaphore = sem;
        mHandle->name      = strdup(name);
        mHandle->creator   = (flags == (O_CREAT|O_EXCL) );
    }
    else
    {
        mHandle->name = 0;
        mHandle->semaphore = new sem_t;

        if(sem_init(mHandle->semaphore, 0, initial) == -1)
        {
            int r = errno;
            delete mHandle->semaphore;
            delete mHandle;

            throw Exception(Z_SOURCEINFO, r, ("无法创建信号量"));
        }
    }
}

Semaphore::~Semaphore()noexcept(false)
{
    if(mHandle->name)
    {
        if(sem_close(mHandle->semaphore) == -1)
            throw Exception(Z_SOURCEINFO, errno, ("无法关闭信号量"));

        // 删除
        if(mHandle->creator)
            sem_unlink(mHandle->name);

        free(mHandle->name);
    }
    else
    {
        if(sem_destroy(mHandle->semaphore) == -1)
            throw Exception(Z_SOURCEINFO, errno, ("无法释放信号量"));

        delete mHandle->semaphore;
    }

    delete mHandle;
}

void Semaphore::wait() noexcept(false)
{
    sem_wait(mHandle->semaphore);
}

bool Semaphore::tryWait() noexcept(false)
{
    return (sem_trywait(mHandle->semaphore) == 0) ? true : false;
}

void Semaphore::post() noexcept(false)
{
_sempost_posix:
    if(sem_post(mHandle->semaphore) == -1)
    {
        int r = errno;
        if(r == EINTR)
            goto _sempost_posix;

        throw Exception(Z_SOURCEINFO, r, ("无法增加信号量"));
    }
}

#endif
