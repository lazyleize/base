/*
* @file    md5.hpp
* @brief   MD5摘要算法
* 摘自QT源码
* @author  Ian Jackson <ian@chiark.greenend.org.uk>
*/

#ifndef __LIBAPS_MD5_HPP__
#define __LIBAPS_MD5_HPP__

#include <base/platform.hpp>
#include <base/digest.hpp>
#include <string>

struct MD5Context;

namespace aps 
{
    //! MD5摘要
    class LIBAPS_API MD5Digest : public Digest
    {
    private:

    public:
        MD5Digest();
        virtual ~MD5Digest();

        void update(const char* buff, size_t len);
        std::string digest();
        void clear();

    private:
        MD5Context* mConext;
    };

}

#endif //!__LIBAPS_MD5_HPP__