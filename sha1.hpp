/*
* @file    sha1.hpp
* @brief   
* @author  Christian Prochnow <cproch@seculogix.de>
*/

#ifndef __LIBAPS_SHA1_HPP__
#define __LIBAPS_SHA1_HPP__

#include <base/platform.hpp>
#include <base/digest.hpp>
#include <string>

namespace aps 
{
    class LIBAPS_API SHA1Digest: public Digest
    {
    public:
        enum 
        {
            DIGEST_SIZE = 20,
            BLOCK_SIZE  = 64
        };

        SHA1Digest();
        SHA1Digest(const SHA1Digest& dig);
        ~SHA1Digest();

        virtual void update(const char* buff, size_t len);
        virtual std::string digest();
        virtual void clear();

        SHA1Digest& operator=(const SHA1Digest& dig);

    private:
        void init();
        void transform(const unsigned char* buffer);

        TUInt64  mByteCount;
        unsigned int  mState[5];
        unsigned char mBlock[64];
    };

}

#endif //!__LIBAPS_SHA1_HPP__
