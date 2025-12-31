/*
 * @file    byte.hpp
 * @brief   位运算函数
 * @author  Christian Prochnow <cproch@seculogix.de>
*/
#ifndef __LIBAPS_BYTEORDER_HPP__
#define __LIBAPS_BYTEORDER_HPP__

#include <base/config_.hpp>


namespace aps {

    //! 判断大端小端
    // http://en.wikipedia.org/wiki/Endianness
    // big endian 返回1, little endian 返回0
    inline int isBigEndian( )
    {
        int c = 1;
        return ((*(char *)&c) != 1);
    }

    static inline TUInt16 swab(TUInt16 __x)
    {
        return ((TUInt16)(
            (((TUInt16)(__x) & (TUInt16)0x00ff) << 8) |
            (((TUInt16)(__x) & (TUInt16)0xff00) >> 8) ));
    }

    static inline TUInt32 swab(TUInt32 __x)
    {
        return ((TUInt32)(
            (((TUInt32)(__x) & (TUInt32)0x000000ff) << 24) |
            (((TUInt32)(__x) & (TUInt32)0x0000ff00) <<  8) |
            (((TUInt32)(__x) & (TUInt32)0x00ff0000) >>  8) |
            (((TUInt32)(__x) & (TUInt32)0xff000000) >> 24) ));
    }

    static inline TUInt64 swab(TUInt64 __x)
    {
        return ((TUInt64)(
            (TUInt64)(((TUInt64)(__x) & (TUInt64)Z_U64BIT_CONSTANT(0x00000000000000ff)) << 56) |
            (TUInt64)(((TUInt64)(__x) & (TUInt64)Z_U64BIT_CONSTANT(0x000000000000ff00)) << 40) |
            (TUInt64)(((TUInt64)(__x) & (TUInt64)Z_U64BIT_CONSTANT(0x0000000000ff0000)) << 24) |
            (TUInt64)(((TUInt64)(__x) & (TUInt64)Z_U64BIT_CONSTANT(0x00000000ff000000)) <<  8) |
            (TUInt64)(((TUInt64)(__x) & (TUInt64)Z_U64BIT_CONSTANT(0x000000ff00000000)) >>  8) |
            (TUInt64)(((TUInt64)(__x) & (TUInt64)Z_U64BIT_CONSTANT(0x0000ff0000000000)) >> 24) |
            (TUInt64)(((TUInt64)(__x) & (TUInt64)Z_U64BIT_CONSTANT(0x00ff000000000000)) >> 40) |
            (TUInt64)(((TUInt64)(__x) & (TUInt64)Z_U64BIT_CONSTANT(0xff00000000000000)) >> 56) ));
    }

    inline TUInt16 cpu_to_le(TUInt16 word)
    {
#ifdef WORDS_BIGENDIAN
        return swab(word);
#else
        return word;
#endif
    }

    inline TUInt16 le_to_cpu(TUInt16 word)
    {
#ifdef WORDS_BIGENDIAN
        return swab(word);
#else
        return word;
#endif
    }

    inline TUInt32 cpu_to_le(TUInt32 dword)
    {
#ifdef WORDS_BIGENDIAN
        return swab(dword);
#else
        return dword;
#endif
    }

    inline TUInt32 le_to_cpu(TUInt32 dword)
    {
#ifdef WORDS_BIGENDIAN
        return swab(dword);
#else
        return dword;
#endif
    }

    inline TUInt64 cpu_to_le(TUInt64 qword)
    {
#ifdef WORDS_BIGENDIAN
        return swab(qword);
#else
        return qword;
#endif
    }

    inline TUInt64 le_to_cpu(TUInt64 qword)
    {
#ifdef WORDS_BIGENDIAN
        return swab(qword);
#else
        return qword;
#endif
    }

    inline TUInt16 cpu_to_be(TUInt16 word)
    {
#ifdef WORDS_BIGENDIAN
        return word;
#else
        return swab(word);
#endif
    }

    inline TUInt16 be_to_cpu(TUInt16 word)
    {
#ifdef WORDS_BIGENDIAN
        return word;
#else
        return swab(word);
#endif
    }

    inline TUInt32 cpu_to_be(TUInt32 dword)
    {
#ifdef WORDS_BIGENDIAN
        return dword;
#else
        return swab(dword);
#endif
    }

    inline TUInt32 be_to_cpu(TUInt32 dword)
    {
#ifdef WORDS_BIGENDIAN
        return dword;
#else
        return swab(dword);
#endif
    }

    inline TUInt64 cpu_to_be(TUInt64 qword)
    {
#ifdef WORDS_BIGENDIAN
        return qword;
#else
        return swab(qword);
#endif
    }

    inline TUInt64 be_to_cpu(TUInt64 qword)
    {
#ifdef WORDS_BIGENDIAN
        return qword;
#else
        return swab(qword);
#endif
    }

}

#endif
