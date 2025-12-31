/*
* @file        charset.hpp
* @brief       字符串编码转换
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#include <base/charset.hpp>
#include <base/toolkit.hpp>
#include <base/strHelper.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace aps;

using namespace std;

// 全局对象
IConv* IConv::gTranslator=NULL;
Mutex IConv::gTranslatorLock;


//! 用于释放全局的编码转换对象
namespace 
{
    class TranslatorAutoRelease
    {
    public:
        TranslatorAutoRelease()
            :m_pPointer(NULL)
        {}

        ~TranslatorAutoRelease()
        { 
            if (m_pPointer!=NULL) 
            {
                delete m_pPointer;
                m_pPointer=NULL;
            }
        }

        IConv*m_pPointer;
    };
}



//! 取默认的编码
static string getDefaultCharset()
{
    std::string LC_CTYPE_;

    const char*pCharset = getenv("LC_CTYPE");
    pCharset = getenv ("LC_ALL");
    if (pCharset == NULL || pCharset[0] == '\0')
    {
        pCharset = getenv ("LC_CTYPE");
        if (pCharset == NULL || pCharset[0] == '\0')
            pCharset = getenv ("LANG");
    }

    pCharset = pCharset==NULL ? "": pCharset;
    LC_CTYPE_ = pCharset;

    if (LC_CTYPE_=="")
        LC_CTYPE_ = "GBK";

    LC_CTYPE_ = StrHelper::toLower(LC_CTYPE_);

    if (LC_CTYPE_.find("utf8")!=string::npos||LC_CTYPE_.find("utf-8")!=string::npos)
        LC_CTYPE_ = "UTF-8";
    else
        LC_CTYPE_ = "GBK";

    return LC_CTYPE_;
}

namespace 
{
    //! 什么也不做的转换类
    class LIBAPS_API NullCharsetTranslator : public IConv
    {
    public:
        NullCharsetTranslator()
            :IConv("","",true)
        {};

        inline string tr(const string& inStr)
        {return inStr;}
    };
}

//! 全局/线程安全的编码转换函数
string IConv::trTS(const string& inStr)
{
    Mutex::ScopedLock lock(IConv::gTranslatorLock);
    IConv*pTranslator = IConv::gTranslator;
    if (pTranslator==NULL)
    {
#ifndef WIN32
        //自适应当前环境变量
        string defCharset = getDefaultCharset();
        if (defCharset=="GBK")
            IConv::setupGlobalTranslator(new NullCharsetTranslator(), true);
        else
            IConv::setupGlobalTranslator(new IConv("GBK", defCharset.c_str()), true);
#else
        IConv::setupGlobalTranslator(new NullCharsetTranslator());
#endif
        pTranslator = IConv::gTranslator;
    }

    return pTranslator->tr(inStr);
}



void IConv::setupGlobalTranslator(IConv*pNewTranslator, bool bIHaveTheLocker)
{
    static TranslatorAutoRelease autoFree;

    // 取锁
    if (!bIHaveTheLocker)
        IConv::gTranslatorLock.lock();

    // 进程退出时自动释放
    if (autoFree.m_pPointer==NULL)
        autoFree.m_pPointer = pNewTranslator;

    // 如果要安装的对象已经存在
    if (pNewTranslator==NULL || pNewTranslator==IConv::gTranslator)
    {
        if (!bIHaveTheLocker)
            IConv::gTranslatorLock.unlock();
        return ;
    }

    // 删除以前的对象, 更新全局变量
    delete IConv::gTranslator;
    IConv::gTranslator = pNewTranslator;

    if (autoFree.m_pPointer!=pNewTranslator)
        autoFree.m_pPointer = pNewTranslator;

    //解锁
    if (!bIHaveTheLocker)
        IConv::gTranslatorLock.unlock();
}

//! GBK转UTF8
std::string aps::gbk2utf8( const string& inStr )
{
    IConv conv("GBK", "UTF-8");
    return conv.tr(inStr);
}

//! UTF8转GBK
std::string aps::utf82gbk( const string& inStr )
{
    IConv conv("UTF-8", "GBK");
    return conv.tr(inStr);
}

#include "utf8.h"
std::wstring LIBAPS_API aps::utf82wstring( const std::string& src )
{
    wstring wstr;
    if (sizeof(wchar_t)==2)
    {
        vector<unsigned short> utf16line;
        utf8::utf8to16(src.begin(), src.end(), back_inserter(utf16line));
        wstr.append(reinterpret_cast<wchar_t*>(&utf16line[0]), utf16line.size());
    }
    else if (sizeof(wchar_t)==4)
    {
        vector<unsigned int> utf32line;
        utf8::utf8to32(src.begin(), src.end(), back_inserter(utf32line));
        wstr.append(reinterpret_cast<wchar_t*>(&utf32line[0]), utf32line.size());
    }
    else 
    {
        logException(Z_SOURCEINFO, -1, "wchar_t not support on this platform!");
    }
    return wstr;
}


std::string LIBAPS_API aps::wstring2utf8( const std::wstring& src )
{
    string ustr;
    vector<unsigned char> utf8line;
    if (sizeof(wchar_t)==2)
    {
        utf8::utf16to8(src.begin(), src.end(), back_inserter(utf8line));
    }
    else if (sizeof(wchar_t)==4)
    {
        utf8::utf32to8(src.begin(), src.end(), back_inserter(utf8line));
    }
    else 
    {
        logException(Z_SOURCEINFO, -1, "wchar_t not support on this platform!");
    }
    ustr.append(reinterpret_cast<char*>(&utf8line[0]), utf8line.size());
    return ustr;
}


