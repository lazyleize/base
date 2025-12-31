/*
* @file        charset.hpp
* @brief       字符串编码转换
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#include <base/charset.hpp>
#include <base/toolkit.hpp>

#ifdef Z_OS_WIN32 

using namespace aps;

using namespace std;

    struct IConv::privateHandle_t
    {
        bool bOpened;
        int iFrom;
        int iTo;

        privateHandle_t()
        {
            bOpened = false;
            iFrom = -1;
            iTo = -1;
        }
    };

    static int getCodePage(string codepage)
    {
        int iCodePage = -1;

        // strToUpper
        for (size_t i=0; i<codepage.length();i++)
        {
            if (codepage[i]>='a' && codepage[i]<='z')
                codepage[i] = 'A' + codepage[i] - 'a';
        }

        if (codepage=="GBK")
            iCodePage = 936;
        else if (codepage=="GB2312")
            iCodePage = 52936;
        else if (codepage=="BIG5")
            iCodePage = 950;
        else if (codepage=="GB18030")
            iCodePage = 54936;
        else if (codepage=="UTF_8"||codepage=="UTF-8"||codepage=="UTF8")
            iCodePage = 65001;
        else 
            iCodePage = -1;

        if (!IsValidCodePage(iCodePage))
            iCodePage = -1;

        return iCodePage;
    }

    IConv::IConv(string fromCode, string toCode, bool /*bErrorContinue*/)
        :mHandle(new privateHandle_t)
    {
        mHandle->bOpened = false;
        mHandle->iFrom = getCodePage(fromCode);
        mHandle->iTo = getCodePage(toCode);

        if (mHandle->iFrom!=-1 && mHandle->iTo!=-1)
            mHandle->bOpened = true;
    }


    bool IConv::isValid()
    {
        return mHandle->bOpened;
    }


    IConv::~IConv()
    {
        delete mHandle;
    }

    string IConv::tr(const string& inStr)
    {
        string toString;
        if (isValid())
        {
            // *** 转换成utf16
            int n = MultiByteToWideChar(mHandle->iFrom, 0, inStr.c_str(), -1, NULL, 0);
            if (n>0)
            {
                wchar_t *pBuffer = new wchar_t[n+1];
                memset(pBuffer, 0x00, sizeof(wchar_t)*(n+1));
                Z_ASSERT( pBuffer!=NULL );
                MultiByteToWideChar(mHandle->iFrom, 0, inStr.c_str(), -1, pBuffer, n);

                //从utf16转 ***
                n = WideCharToMultiByte(mHandle->iTo, 0, pBuffer, -1, 0, 0, 0, 0 );
                if (n>0)
                {
                    char *pBuffer0 = new char[n+1];
                    memset(pBuffer0, 0x00, sizeof(char)*(n+1));
                    Z_ASSERT( pBuffer0!=NULL );
                    WideCharToMultiByte(mHandle->iTo, 0, pBuffer, -1, pBuffer0, n, 0, 0);

                    toString = pBuffer0;
                    delete []pBuffer0;
                }
                delete []pBuffer;
            }
        }
        return toString;
    }

#endif
