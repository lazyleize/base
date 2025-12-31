/*
* @file        charset.hpp
* @brief       字符串编码转换
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#include <base/charset.hpp>
#include <base/toolkit.hpp>

#ifdef Z_OS_LINUX

#include <locale.h>
#include <stdio.h>
#include <errno.h>
#include <iconv.h>
#include <cstring>

namespace aps {

static const iconv_t invalid = reinterpret_cast<iconv_t>(-1);
using namespace std;

struct IConv::privateHandle_t
{
    iconv_t cd;
    bool bOpened;

    string fromCode;
    string toCode;
};


IConv::IConv(string fromCode, string toCode, bool /*bErrorContinue*/)
:mHandle(new privateHandle_t)
{
    mHandle->cd = iconv_open(toCode.c_str(), fromCode.c_str());
    if (mHandle->cd == invalid)
        return;

    mHandle->toCode = toCode;
    mHandle->fromCode = fromCode;
    mHandle->bOpened = true;
}


bool IConv::isValid()
{
    return mHandle->bOpened;
}


IConv::~IConv()
{
    if (mHandle->cd != invalid)
        iconv_close(mHandle->cd);
    delete mHandle;
}

string IConv::tr(const string& inStr)
{
    string toString;
    if (isValid())
    {
#ifdef Z_HAVE_GNU_LIBICONV
        // GNU libiconv
        const char *inp = reinterpret_cast<const char *>(inStr.data());
#else
        // POSIX compliant iconv(3)
        char *inp = reinterpret_cast<char *>(const_cast<string::value_type *>(inStr.data()));
#endif
        const string::size_type in_size = inStr.size() * sizeof(string::value_type);
        string::size_type in_bytes = in_size;

        toString.resize(in_size);

        char *outp =reinterpret_cast<char *>(const_cast<string::value_type *>(toString.data()));
        string::size_type out_size = toString.size() * sizeof(string::value_type);
        string::size_type out_bytes = out_size;

        do 
        {
#ifdef Z_HAVE_GNU_LIBICONV 
            size_t l = iconv(mHandle->cd, (char**)&inp, &in_bytes, (char**)&outp, &out_bytes);
#else 
            size_t l = iconv(mHandle->cd, &inp, &in_bytes, &outp, &out_bytes);
#endif
            if (l == (size_t) -1) 
            {
                switch (errno)
                {
                case EILSEQ:
                case EINVAL:
                    {
                        const string::size_type off = in_size - in_bytes + 1;
#ifdef Z_HAVE_GNU_LIBICONV 
                        // GNU libiconv
                        inp = reinterpret_cast<const char *>(inStr.data()) + off;
#else 
                        // POSIX compliant iconv(3)
                        inp = reinterpret_cast<char *>(const_cast<string::value_type *>(inStr.data()));
#endif
                        in_bytes = in_size - off;
                        break;
                    }
                case E2BIG:
                    {
                        const string::size_type off = out_size - out_bytes;
                        toString.resize(toString.size() * 2);
                        out_size = toString.size() * sizeof(string::value_type);

                        outp = reinterpret_cast<char *>(const_cast<string::value_type *>(toString.data())) + off;
                        out_bytes = out_size - off;
                        break;
                    }
                default:
                    toString = string();
                    return toString;
                }
            }
        } while (in_bytes != 0);

    }

    if (toString.length()!=strlen(toString.c_str()))
        toString = toString.c_str();

    return toString;
}

}

#endif
