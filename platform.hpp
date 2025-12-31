/*
 * @file        platform.hpp
 * @brief       尽可能屏蔽平台差
 * @author      leize<leize@xgd.com>
 *
 * @copyright   xgd
*/
#ifndef __LIBAPS_PLATFORM_HPP__
#define __LIBAPS_PLATFORM_HPP__

#include <base/config_.hpp>

#ifdef _WIN32
#include <base/platform.win32.hpp>
#elif defined(Z_OS_LINUX)
#else
#endif

#endif // !__LIBAPS_PLATFORM_HPP__
