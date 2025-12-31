/*
 * @file    ebcdic.hpp
 * @brief   ebcdic与ascii转换
 *
 * 移植自openssl(crypto/ebcdic.h)
 * 
*/

#ifndef __LIBAPS_EBCDIC_HPP__
#define __LIBAPS_EBCDIC_HPP__

#include <base/config_.hpp>
#include <sys/types.h>
#include <string>

namespace aps
{
    extern const unsigned char os_toascii[256];
    extern const unsigned char os_toebcdic[256];
    LIBAPS_API void *ebcdic2ascii(void *dest, const void *srce, size_t count);
    LIBAPS_API void *ascii2ebcdic(void *dest, const void *srce, size_t count);

    LIBAPS_API std::string ebcdic2ascii(const std::string& input);
    LIBAPS_API std::string ascii2ebcdic(const std::string& input);
}

#endif // ! __LIBAPS_EBCDIC_HPP__
