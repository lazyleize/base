/*
 * @file        event.cpp
 * @brief       事件封装
 * 摘自POCO
 * @author      sunu<pansunyou@gmail.com>
 * @copyright   xgd
*/
#include <base/event.hpp>
#include <base/platform.hpp>
#include <base/charset.hpp>
#include <base/exception.hpp>

#ifndef _WIN32

#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

using namespace aps;

struct Event::ev_handle_t
{
    volatile bool   _auto;
    volatile bool   _state;
    pthread_mutex_t _mutex;
    pthread_cond_t  _cond;

    ev_handle_t()
    {
        _auto = true;
        _state = false;
    }
};

Event::Event(bool autoReset)
:mHandle(new ev_handle_t)
{
    if (pthread_mutex_init(&mHandle->_mutex, NULL))
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "cannot create event (mutex)");
    }
    if (pthread_cond_init(&mHandle->_cond, NULL))
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "cannot create event (condition)");
    }
}


Event::~Event()
{
    pthread_cond_destroy(&mHandle->_cond);
    pthread_mutex_destroy(&mHandle->_mutex);
}


void Event::wait()
{
    if (pthread_mutex_lock(&mHandle->_mutex))
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "wait for event failed (lock)");
    }
    while (!mHandle->_state) 
    {
        if (pthread_cond_wait(&mHandle->_cond, &mHandle->_mutex))
        {
            pthread_mutex_unlock(&mHandle->_mutex);
            {
                int r = errno;
                throw Exception(Z_SOURCEINFO, r, "wait for event failed");
            }
        }
    }
    if (mHandle->_auto)
        mHandle->_state = false;
    pthread_mutex_unlock(&mHandle->_mutex);
}


bool Event::wait(long milliseconds)
{
    int rc = 0;
    struct timespec abstime;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    abstime.tv_sec  = tv.tv_sec + milliseconds / 1000;
    abstime.tv_nsec = tv.tv_usec*1000 + (milliseconds % 1000)*1000000;
    if (abstime.tv_nsec >= 1000000000)
    {
        abstime.tv_nsec -= 1000000000;
        abstime.tv_sec++;
    }

    if (pthread_mutex_lock(&mHandle->_mutex) != 0)
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "wait for event failed(lock)");
    }

    while (!mHandle->_state) 
    {
        if ((rc = pthread_cond_timedwait(&mHandle->_cond, &mHandle->_mutex, &abstime)))
        {
            if (rc == ETIMEDOUT) break;
            pthread_mutex_unlock(&mHandle->_mutex);
            {
                int r = errno;
                throw Exception(Z_SOURCEINFO, r, "wait for event failed");
            }
        }
    }

    if (rc == 0 && mHandle->_auto) mHandle->_state = false;
    pthread_mutex_unlock(&mHandle->_mutex);
    return rc == 0;
}


void Event::set()
{
    if (pthread_mutex_lock(&mHandle->_mutex))    
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "cannot signal event (lock)");
    }
    mHandle->_state = true;
    if (pthread_cond_broadcast(&mHandle->_cond))
    {
        int r = errno;
        pthread_mutex_unlock(&mHandle->_mutex);
        throw Exception(Z_SOURCEINFO, r, "cannot signal event");
    }
    pthread_mutex_unlock(&mHandle->_mutex);
}


void Event::reset()
{
    if (pthread_mutex_lock(&mHandle->_mutex))    
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "cannot reset event");
    }
    mHandle->_state = false;
    pthread_mutex_unlock(&mHandle->_mutex);
}

#endif
