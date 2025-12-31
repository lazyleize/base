/*
* @file        charset.hpp
* @brief       字符串编码转换
* @author      雷泽<leize@xgd.com>
* @version     1.0
* @copyright   leize
*/
#ifndef __LIBAPS_CHARSET_HPP__
#define __LIBAPS_CHARSET_HPP__
#include <base/mutex.hpp>
#include <iostream>
#include <string>
#include <string.h>
#include <cstdio>
#include <vector>

using namespace std;
namespace aps
{

    //! 编码转换类
    class  LIBAPS_API IConv
    {
    public:

        IConv(string fromCode, string toCode, bool bErrorContinue=true);
        virtual ~IConv();

        bool isValid();
        virtual string tr(const string& inStr);


        static string trTS(const string& inStr);
        static void setupGlobalTranslator(IConv* pTranslator, bool bIHaveTheLocker=false);
    private:
        IConv(const IConv&other);
        IConv& operator=(const IConv& other);

        struct privateHandle_t;
        privateHandle_t *mHandle;

        //全局共用的转换对象
        static IConv* gTranslator;
		static Mutex gTranslatorLock;
    };

    typedef unsigned long UTF32;    /* at least 32 bits */
    typedef unsigned short UTF16;   /* at least 16 bits */
    typedef unsigned char UTF8;     /* typically 8 bits */

    typedef enum
    {
        conversionOK = 0,     /* conversion successful */
        sourceExhausted,  /* partial character in source, but hit end */
        targetExhausted,  /* insuff. room in target for conversion */
        sourceIllegal     /* source sequence is illegal/malformed */
    } ConvtRet_t;

    typedef enum
    {
        strictConversion = 0,
        lenientConversion
    } ConvtFlags_t;

    //! GBK转UTF8
    std::string LIBAPS_API gbk2utf8( const string& inStr );
    
    //! UTF8转GBK
    std::string LIBAPS_API utf82gbk( const string& inStr );

    //  utf-8转换成wstring
    std::wstring LIBAPS_API utf82wstring(const std::string& src);
    
    //! wstring转换成utf-8
    std::string LIBAPS_API wstring2utf8(const std::wstring& src);


    //! wstring转换成utf-8


    //! 编码转换宏
    #define _G2A(x) aps::IConv::trTS(x).c_str()
    #define _G2U(x) gbk2utf8(x).c_str()
    #define _U2G(x) utf82gbk(x).c_str()

}

#endif // !__LIBAPS_CHARSET_HPP__
