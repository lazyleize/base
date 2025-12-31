/*
* @file        toolkit.cpp
* @brief       常用工具类
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#include <base/toolkit.hpp>
#include <base/charset.hpp>
#include <base/mutex.hpp>
#include <base/logger.hpp>

#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <errno.h>

#ifdef Z_OS_WIN32
#include <process.h>
#endif

using namespace aps;
using namespace std;

namespace aps
{
	void Toolkit::assertTrue(int line, const char*file, const char*valStr, bool val)
    {
        if (!val)
        {
            fprintf(stderr, "Assertion failed: %s, file %s, line %d\r\n",
                valStr, file, line);
            exit(-1);
        }
    }

	// KB/MB/GB/TB/PB/EB/ZB/YB/NB/DB/CB
	string Toolkit::formatFileSize(double _fileSize,size_t suffixLen)
	{
		static const char* unitList[] = {"B","KB","MB","GB","TP","PB","EB","ZB","YB","NB","CB"}; 
		double fileSize = _fileSize;
        int unitIdx = 0;
		for(;;)
		{
			if(fileSize / 1024 > 1.00000)
			{
				unitIdx ++;
				fileSize /= 1024;
			}
			else
				break;
		}

		if(unitIdx < 0)
			unitIdx = 0;

		int sizeOfArry = sizeof(unitList)/sizeof(unitList[0]);
		if(unitIdx >= sizeOfArry)
			return "***";

		double deval = 1;
		for(int i = unitIdx;i > 0;i--)
			deval *= 1024;

		fileSize = (double)_fileSize;
		fileSize /= deval;

		char szBuffer[100];
        char szFormat[100];
        memset(szFormat, 0, sizeof(szFormat));
        sprintf(szFormat, "%%0.%dlf %%s", (int)suffixLen);

        memset(szBuffer, 0, sizeof(szBuffer));
        sprintf(szBuffer, szFormat, fileSize, unitList[unitIdx]);
        return szBuffer;
	}
	string Toolkit::bin2hex(const void* inDat,size_t length,bool bUpper)
	{
		Z_ASSERT((inDat != NULL));
		string hex;
		hex.resize(length*2+1,0);

		Toolkit::bin2hex(inDat,length,(unsigned char*)hex.c_str(), length*2+1, bUpper);

		return hex;
	}

	void Toolkit::bin2hex( const void*inDat, size_t inLength, unsigned char*outBuf, size_t outBufSize, bool bUpper/*=false*/ )
    {
        const char* ConvertUpper = "0123456789ABCDEF"; 
        const char* ConvertLower = "0123456789abcdef"; 

        unsigned char *pDat = (unsigned char *)inDat;
        int pos = 0;

        if (bUpper)
        {
            for (size_t i=0; i<inLength; i++, pos+=2)
            {
                outBuf[pos] = ConvertUpper[ (unsigned char)pDat[i] >> 4 ];
                outBuf[pos+1] = ConvertUpper[ (unsigned char)pDat[i] & 0xf ];
            }
            outBuf[pos] = 0;
        }
        else 
        {
            for (size_t i=0; i<inLength; i++, pos+=2)
            {
                outBuf[pos] = ConvertLower[ (unsigned char)pDat[i] >> 4 ];
                outBuf[pos+1] = ConvertLower[ (unsigned char)pDat[i] & 0xf ];
            }
            outBuf[pos] = 0;
        }
    }

	bool Toolkit::hex2bin(string inHexStr, void*outBuffer, size_t bufferLen, bool bZeroTail)
    {
        Z_ASSERT( (outBuffer != NULL) && (bufferLen>0) );

        if (inHexStr.length() % 2 !=0)
            return false;

        unsigned char charVal;
        char szBuffer[3] = {0,0,0};
        unsigned char *pBuffer = (unsigned char *)outBuffer;

        size_t i=0;
        for (; i<inHexStr.length()/2 && i<bufferLen; i++)
        {
            charVal = 0;
            memcpy(szBuffer, inHexStr.c_str() + i*2, 2);

            if (szBuffer[0] >= '0' && szBuffer[0] <= '9')
                charVal = szBuffer[0] - '0';
            else if (szBuffer[0] >= 'A' && szBuffer[0] <= 'F')
                charVal = szBuffer[0] - 'A' + 10;
            else
                charVal = szBuffer[0] - 'a' + 10;

            charVal *= 16;

            if (szBuffer[1] >= '0' && szBuffer[1] <= '9')
                charVal += szBuffer[1] - '0';
            else if (szBuffer[1] >= 'A' && szBuffer[1] <= 'F')
                charVal += szBuffer[1] - 'A' + 10;
            else
                charVal += szBuffer[1] - 'a' + 10;

            pBuffer[i] = charVal;
        }

        if (bZeroTail && i<bufferLen)
            pBuffer[i] = '\0';

        return true;
    }

	string Toolkit::formatError(int r)
    {
        string errString  = ("未知");
#ifdef Z_OS_WIN32
        DWORD nErrorNo = r;
        if (0xFFFFFFFF==r)
            nErrorNo = GetLastError(); // 得到错误代码
        LPSTR lpBuffer = NULL;    
        FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER  | 
            FORMAT_MESSAGE_IGNORE_INSERTS  | 
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            nErrorNo, // 此乃错误代码，通常在程序中可由 GetLastError()得之
            LANG_NEUTRAL,
            (char*) &lpBuffer,
            0 ,
            NULL);
        errString  =  lpBuffer !=NULL ?  lpBuffer : ("无");
        LocalFree(lpBuffer);
#else
        int errsv = errno;
        char szBuffer[512];
        memset(szBuffer, 0x00, sizeof(szBuffer));
        errString = strerror_r(errsv, szBuffer, sizeof(szBuffer));
#endif
        return errString;
    }


    int Toolkit::getCurrentProcessId()
    {
#ifdef Z_OS_WIN32
        return GetCurrentProcessId();
#else
        return getpid();
#endif 

    }

    double Toolkit::random(double start, double end)
    {
        static bool bInit = false;
        if (!bInit)
        {
            bInit = true;
            srand(unsigned(time(0)));
        }
        return start+(end-start)*rand()/(RAND_MAX + 1.0);
    }


    int Toolkit::getLastErrorNo()
    {
#ifndef _WIN32
        return errno;
#else 
        return GetLastError();
#endif 
    }
}
