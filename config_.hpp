/*
 * @file    config_.hpp
 * @brief   类型编译选项配置文件(仅限类库内部使用)
 *          
 * @author      leize<leize@xgd.com>
 * @date        2021/01/01
 * @copyright   Copyright (c) 2021 xgd 
*/
#ifndef __LIBAPS__CONFIG_HPP__
#define __LIBAPS__CONFIG_HPP__

#ifndef LIBAPS_EXP_DECL
#define LIBAPS_EXP_DECL

#  ifdef _WIN32
#    if defined(_WINDLL) && !defined(USING_LIBAPS_STATIC) && !defined(USING_LIBAPS_DLL)
#      define LIBAPS_API       __declspec(dllexport)
#    elif defined(USING_LIBAPS_DLL) && !defined(USING_LIBAPS_STATIC)
#      define LIBAPS_API       __declspec(dllimport)
#    else
#      define LIBAPS_API
#    endif
#  else
#    define LIBAPS_API
#  endif

#ifdef _WIN32
#define LIBAPS_EXPORT __declspec(dllexport)  
#define LIBAPS_IMPORT __declspec(dllimport)
#else
#define LIBAPS_EXPORT 
#define LIBAPS_IMPORT 
#endif

#endif //! LIBAPS_EXP_DECL


//////////////////////////////////////////////////////////////////////////

#ifndef Z_DEFINE_INT64
#define Z_DEFINE_INT64
#if defined(_WIN32)
    typedef __int64 TInt64;
    typedef unsigned __int64 TUInt64;
#else
    typedef long long TInt64;
    typedef unsigned long long TUInt64;
#endif 
#endif 

    typedef char TInt8;
    typedef unsigned char TUInt8;

    typedef short TInt16;
    typedef unsigned short TUInt16;

    typedef int TInt32;
    typedef unsigned int TUInt32;


//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif 
# pragma warning (disable:4290)
# pragma warning (disable:4996)
# pragma warning (disable:4355)
# pragma warning (disable:4251)
# define Z_OS_WIN32
#else 
# define Z_OS_LINUX
#endif 

#ifdef _MSC_VER
#define Z_64BIT_CONSTANT(x)     x##i64
#define Z_U64BIT_CONSTANT(x)    x##ui64
#else
#define Z_64BIT_CONSTANT(x)     x##ll
#define Z_U64BIT_CONSTANT(x)    x##ull
#endif

#ifdef __cplusplus

//! 全局命名空间(不污染环境)
namespace aps
{
    //
}

#endif

#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
//#define throw(Exception) noexcept(false)
#endif

#endif // !__LIBAPS__CONFIG_HPP__
