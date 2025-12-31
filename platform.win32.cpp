/*
 * @file    platform.cpp
 * @brief   屏蔽系统差异
 * @author  leize<leize@xgd.com>
 *
*/
#include <base/config_.hpp>
#include <base/platform.hpp>

#ifdef Z_OS_WIN32
#ifdef _WINSOCKAPI_
#undef _WINSOCKAPI_
#endif 
#include <time.h>
#include <winsock.h>

namespace aps {

int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm _tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    _tm.tm_year  = wtm.wYear - 1900;
    _tm.tm_mon   = wtm.wMonth - 1;
    _tm.tm_mday  = wtm.wDay;
    _tm.tm_hour  = wtm.wHour;
    _tm.tm_min   = wtm.wMinute;
    _tm.tm_sec   = wtm.wSecond;
    _tm.tm_isdst = -1;
    clock = mktime(&_tm);
    tp->tv_sec = (long)clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}

}

#endif
