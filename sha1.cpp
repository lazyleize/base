/*
* @file    sha1.hpp
* @brief   哈希算法摘要
* @author  Christian Prochnow <cproch@seculogix.de>
*/

#include <base/sha1.hpp>
#include <base/byteorder.hpp>
#include <cstdio>
#include <cstring>

using namespace aps;

#define z__rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
#define z__blk0(i) block32[i]
#define z__blk(i) (block32[i&15] = z__rol(block32[(i+13)&15]^block32[(i+8)&15] ^block32[(i+2)&15]^block32[i&15],1))
#define z__R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+z__blk0(i)+0x5A827999+z__rol(v,5);w=z__rol(w,30);
#define z__R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+z__blk(i)+0x5A827999+z__rol(v,5); w=z__rol(w,30);
#define z__R2(v,w,x,y,z,i) z+=(w^x^y)+z__blk(i)+0x6ED9EBA1+z__rol(v,5);w=z__rol(w,30);
#define z__R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+z__blk(i)+0x8F1BBCDC+z__rol(v,5); w=z__rol(w,30);
#define z__R4(v,w,x,y,z,i) z+=(w^x^y)+z__blk(i)+0xCA62C1D6+z__rol(v,5);w=z__rol(w,30);

    SHA1Digest::SHA1Digest()
    {
        init();
    }

    SHA1Digest::SHA1Digest(const SHA1Digest& dig)
    {
        *this = dig;
    }

    SHA1Digest::~SHA1Digest()
    {
    }

    void SHA1Digest::clear()
    {
        init();
    }

    SHA1Digest& SHA1Digest::operator=(const SHA1Digest& dig)
    {
        mByteCount = dig.mByteCount;
        mState[0] = dig.mState[0];
        mState[1] = dig.mState[1];
        mState[2] = dig.mState[2];
        mState[3] = dig.mState[3];
        mState[4] = dig.mState[4];

        for(unsigned int i=0; i < BLOCK_SIZE; i++)
            mBlock[i] = dig.mBlock[i];

        return *this;
    }

    void SHA1Digest::init()
    {
        mByteCount = 0;
        mState[0] = 0x67452301;
        mState[1] = 0xEFCDAB89;
        mState[2] = 0x98BADCFE;
        mState[3] = 0x10325476;
        mState[4] = 0xC3D2E1F0;

        for(unsigned int i=0; i < BLOCK_SIZE; i++)
            mBlock[i] = 0;
    }

    void SHA1Digest::transform(const unsigned char* buffer)
    {
        unsigned int block32[16];

        /* convert/copy data to workspace */
        for (unsigned int i = 0; i < BLOCK_SIZE/sizeof(unsigned int); i++)
            block32[i] = be_to_cpu(((const unsigned int*)buffer)[i]);

        /* Copy context->state[] to working vars */
        register unsigned int a = mState[0];
        register unsigned int b = mState[1];
        register unsigned int c = mState[2];
        register unsigned int d = mState[3];
        register unsigned int e = mState[4];

        /* 4 rounds of 20 operations each. Loop unrolled. */
        z__R0(a,b,c,d,e, 0); z__R0(e,a,b,c,d, 1); z__R0(d,e,a,b,c, 2); z__R0(c,d,e,a,b, 3);
        z__R0(b,c,d,e,a, 4); z__R0(a,b,c,d,e, 5); z__R0(e,a,b,c,d, 6); z__R0(d,e,a,b,c, 7);
        z__R0(c,d,e,a,b, 8); z__R0(b,c,d,e,a, 9); z__R0(a,b,c,d,e,10); z__R0(e,a,b,c,d,11);
        z__R0(d,e,a,b,c,12); z__R0(c,d,e,a,b,13); z__R0(b,c,d,e,a,14); z__R0(a,b,c,d,e,15);
        z__R1(e,a,b,c,d,16); z__R1(d,e,a,b,c,17); z__R1(c,d,e,a,b,18); z__R1(b,c,d,e,a,19);
        z__R2(a,b,c,d,e,20); z__R2(e,a,b,c,d,21); z__R2(d,e,a,b,c,22); z__R2(c,d,e,a,b,23);
        z__R2(b,c,d,e,a,24); z__R2(a,b,c,d,e,25); z__R2(e,a,b,c,d,26); z__R2(d,e,a,b,c,27);
        z__R2(c,d,e,a,b,28); z__R2(b,c,d,e,a,29); z__R2(a,b,c,d,e,30); z__R2(e,a,b,c,d,31);
        z__R2(d,e,a,b,c,32); z__R2(c,d,e,a,b,33); z__R2(b,c,d,e,a,34); z__R2(a,b,c,d,e,35);
        z__R2(e,a,b,c,d,36); z__R2(d,e,a,b,c,37); z__R2(c,d,e,a,b,38); z__R2(b,c,d,e,a,39);
        z__R3(a,b,c,d,e,40); z__R3(e,a,b,c,d,41); z__R3(d,e,a,b,c,42); z__R3(c,d,e,a,b,43);
        z__R3(b,c,d,e,a,44); z__R3(a,b,c,d,e,45); z__R3(e,a,b,c,d,46); z__R3(d,e,a,b,c,47);
        z__R3(c,d,e,a,b,48); z__R3(b,c,d,e,a,49); z__R3(a,b,c,d,e,50); z__R3(e,a,b,c,d,51);
        z__R3(d,e,a,b,c,52); z__R3(c,d,e,a,b,53); z__R3(b,c,d,e,a,54); z__R3(a,b,c,d,e,55);
        z__R3(e,a,b,c,d,56); z__R3(d,e,a,b,c,57); z__R3(c,d,e,a,b,58); z__R3(b,c,d,e,a,59);
        z__R4(a,b,c,d,e,60); z__R4(e,a,b,c,d,61); z__R4(d,e,a,b,c,62); z__R4(c,d,e,a,b,63);
        z__R4(b,c,d,e,a,64); z__R4(a,b,c,d,e,65); z__R4(e,a,b,c,d,66); z__R4(d,e,a,b,c,67);
        z__R4(c,d,e,a,b,68); z__R4(b,c,d,e,a,69); z__R4(a,b,c,d,e,70); z__R4(e,a,b,c,d,71);
        z__R4(d,e,a,b,c,72); z__R4(c,d,e,a,b,73); z__R4(b,c,d,e,a,74); z__R4(a,b,c,d,e,75);
        z__R4(e,a,b,c,d,76); z__R4(d,e,a,b,c,77); z__R4(c,d,e,a,b,78); z__R4(b,c,d,e,a,79);

        /* Add the working vars back into context.state[] */
        mState[0] += a;
        mState[1] += b;
        mState[2] += c;
        mState[3] += d;
        mState[4] += e;

        /* Wipe variables */
        a = b = c = d = e = 0;
        memset(block32, 0x00, BLOCK_SIZE);
    }

    void SHA1Digest::update(const char* data, size_t len)
    {
        unsigned i, j;

        j = (mByteCount >> 3) & 0x3f;

        mByteCount += len << 3;

        if((j + len) > 63)
        {
            memcpy(&mBlock[j], data, (i = 64-j));
            transform(mBlock);
            for( ; i + 63 < len; i += 64)
                transform((unsigned char*)&data[i]);
            j = 0;
        }
        else
            i = 0;

        memcpy(&mBlock[j], &data[i], len - i);
    }

    string SHA1Digest::digest()
    {
        const static unsigned char padding[64] = { 0x80, };
        unsigned int i, j, index, padLen;
        TUInt64 t;
        unsigned char bits[8] = { 0, };
        unsigned char digest[20];

        t = mByteCount;
        bits[7] = 0xff & t; t>>=8;
        bits[6] = 0xff & t; t>>=8;
        bits[5] = 0xff & t; t>>=8;
        bits[4] = 0xff & t; t>>=8;
        bits[3] = 0xff & t; t>>=8;
        bits[2] = 0xff & t; t>>=8;
        bits[1] = 0xff & t; t>>=8;
        bits[0] = 0xff & t;

        /* Pad out to 56 mod 64 */
        index = (mByteCount >> 3) & 0x3f;
        padLen = (index < 56) ? (56 - index) : ((64+56) - index);

        SHA1Digest work(*this);

        work.update((const char*)padding, padLen);

        /* Append length */
        work.update((const char*)bits, sizeof(bits));

        /* Store state in digest */
        for (i = j = 0; i < 5; i++, j += 4)
        {
            unsigned int t2 = work.mState[i];
            digest[j+3] = t2 & 0xff; t2>>=8;
            digest[j+2] = t2 & 0xff; t2>>=8;
            digest[j+1] = t2 & 0xff; t2>>=8;
            digest[j  ] = t2 & 0xff;
        }

        const char* p = (const char*)digest;
        char szBuffer[42];
        for(i = 0; i < DIGEST_SIZE; i++)
        {
            unsigned char ch = *p++;
            sprintf(szBuffer+2*i, "%02x", ch);
        }
        szBuffer[40] = '\0';
        return szBuffer;
    }
