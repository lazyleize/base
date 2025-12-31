/*
* @file        stream.hpp
* @brief       输入输出设备基类
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_IODEVICE_HPP__
#define __LIBAPS_IODEVICE_HPP__

#include <base/platform.hpp>
#include <base/exception.hpp>

namespace aps 
{
    //! 流对象基类
    class LIBAPS_API Stream
    {
    public:
        Stream(){};
        virtual ~Stream(){};

        virtual void close() = 0;
        virtual bool isOpened() = 0;
        
        virtual size_t write(const char* buffer, size_t count) = 0;
        virtual size_t read(char* buffer, size_t count) = 0;
        virtual bool seek(off_t offset, int mode)= 0;
        
    protected:
    };
}

#endif // !__LIBAPS_IODEVICE_HPP__
