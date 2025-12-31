/*
 * @file        base64.hpp
 * @brief       base64
 *
 * 摘自C++ Sockets软件包
 *
 * @author      grymse@alhem.net
 * @copyright   xgd
*/
#ifndef __LIBAPS_BASE64_HPP__
#define __LIBAPS_BASE64_HPP__

#include <base/platform.hpp>
#include <sstream>
#include <string>

namespace aps 
{
    //! Base64编码解码
    class LIBAPS_API Base64
    {
    public:
        Base64();

        void encode(FILE *, std::string& , bool add_crlf = false);
        void encode(const std::string&, std::string& , bool add_crlf = false);
        void encode(const char *, size_t, std::string& , bool add_crlf = false);
        void encode(const unsigned char *, size_t, std::string& , bool add_crlf = false);

        void decode(const std::string&, std::string& );
        void decode(const std::string&, unsigned char *, size_t&);

        size_t decodeLength(const std::string& );

    private:
        Base64(const Base64& ) {}
        Base64& operator=(const Base64& ) { return *this; }

        static const char *bstr;
        static const char rstr[128];
    };
}

#endif // !__LIBAPS_BASE64_HPP__
