/*
* @file        sourceInfo.hpp
* @brief       代码所在行信息,用于日志和异常类
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_SOURCEINFO_HPP__
#define __LIBAPS_SOURCEINFO_HPP__

#include <base/platform.hpp>

namespace aps
{
	// GNU C++ 编译器
#ifdef __GNUC__
#define Z_PRETTY_FUNCTION __PRETTY_FUNCTION__
    // Borland C++
#elif defined(__BORLANDC__)
#define Z_PRETTY_FUNCTION __FUNC__
    // Microsoft C++ 编译器 
#elif defined(_MSC_VER_)
#if _MSC_VER_ >= 1300
#define Z_PRETTY_FUNCTION __FUNCDNAME__
#else
#define Z_PRETTY_FUNCTION __FUNCTION__
#endif
    // 其它
#else
#define Z_PRETTY_FUNCTION __FUNCTION__
#endif

    #define Z_SOURCEINFO aps::SourceInfo(__FILE__,__LINE__, __FUNCTION__)

    //! 代码位置跟踪类
    class LIBAPS_API SourceInfo
    {
    public:
        SourceInfo() throw();

        SourceInfo(const SourceInfo& si) throw();

        SourceInfo(const char* _file, unsigned int _line, const char* _func) throw();

        const char* file() const throw();

        unsigned int line() const throw();

        const char* func() const throw();

        SourceInfo& operator=(const SourceInfo& si);
    private:
        char mSzFile[512];
        char mSzFunc[256];
        unsigned int mLine;
    };
}

#endif // !__LIBAPS_SOURCEINFO_HPP__