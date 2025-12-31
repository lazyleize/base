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
#include <base/toolkit.hpp>

#ifdef Z_OS_WIN32

using namespace aps;

struct Event::ev_handle_t
{
    HANDLE event_;

    ev_handle_t()
    {
        event_ = INVALID_HANDLE_VALUE;
    }
};


Event::Event(bool autoReset)
:mHandle(new ev_handle_t)
{
    mHandle->event_ = CreateEventW(NULL, autoReset ? FALSE : TRUE, FALSE, NULL);
    if (!mHandle->event_)
    {
        int r = GetLastError();
        throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
    }
}


Event::~Event()
{
    if (mHandle->event_!=INVALID_HANDLE_VALUE)
        CloseHandle(mHandle->event_);
}


void Event::wait()
{
    switch (WaitForSingleObject(mHandle->event_, INFINITE))
    {
    case WAIT_OBJECT_0:
        return;
    default:
        {
            int r = GetLastError();
            throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
        }
    }
}


bool Event::wait(long milliseconds)
{
    switch (WaitForSingleObject(mHandle->event_, milliseconds + 1))
    {
    case WAIT_TIMEOUT:
        return false;
    case WAIT_OBJECT_0:
        return true;
    default:
        {
            int r = GetLastError();
            throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
        }
    }
}


void Event::set()
{
    if (!SetEvent(mHandle->event_))
    {
        int r = GetLastError();
        throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
    }
}


void Event::reset()
{
    if (!ResetEvent(mHandle->event_))
    {
       int r = GetLastError();
       throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
    }
}


#endif
