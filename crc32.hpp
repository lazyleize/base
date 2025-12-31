/*
* @file    crc32.hpp
* @brief   CRC32摘要算法
* 摘自QT源码
* @author  Ian Jackson <ian@chiark.greenend.org.uk>
*/

#ifndef __LIBAPS_CRC32_HPP__
#define __LIBAPS_CRC32_HPP__

#include <base/platform.hpp>
#include <base/digest.hpp>
#include <string>

namespace aps 
{

    //! CRC32 摘要
    class LIBAPS_API Crc32Digest : public Digest
    {
    public:
        Crc32Digest();
        virtual ~Crc32Digest();

        void update(const char* buff, size_t len);
        std::string digest();
        void clear();

        inline unsigned int sum()
        { return mSum; }

    private:
        unsigned int mSum;
    };

}

#endif //!__LIBAPS_CRC32_HPP__
