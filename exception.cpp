/*
 * @file        exception.hpp
 * @brief       简单的异常类
 * @author      zny
 *
 * @copyright   xgd
*/
#include <base/exception.hpp>
#include <base/logger.hpp>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cstdarg>

#ifdef WIN32
#define snprintf _snprintf
#endif

using namespace aps;
using namespace std;

Exception::Exception()
{
	mErrMsg = "";
	mIntErrCode = -1;
}

Exception::Exception(const char* lpFile, int iLine, const char*lpFunc, int iCode, const string& errMsg)
{
    lpFile = lpFile!=NULL?lpFile:"";
    lpFunc = lpFunc!=NULL?lpFunc:"";

	mErrMsg = errMsg;
	mIntErrCode = iCode;
	mSrcInfo = SourceInfo(lpFile,iLine,lpFunc);
}

Exception::Exception(SourceInfo sourceInfo, int iCode, const string& errMsg)
{
    mSrcInfo = SourceInfo(sourceInfo);
    mIntErrCode = iCode;
    mErrMsg = errMsg;
}

aps::Exception::Exception( SourceInfo sourceInfo, const string& sCode, const string& errMsg )
{
    mSrcInfo = SourceInfo(sourceInfo);
    mIntErrCode = 0;
    mErrMsg = errMsg;
    mStrCode = sCode;
}

Exception::Exception(const Exception& e)
{
    mSrcInfo = e.mSrcInfo;
    mErrMsg = e.mErrMsg;
    mIntErrCode = e.mIntErrCode;
    mStrCode = e.mStrCode;
}

Exception& Exception::operator=(const Exception& e)
{
    if (&e != this)
    {
        mSrcInfo = e.mSrcInfo;
        mErrMsg = e.mErrMsg;
        mIntErrCode = e.mIntErrCode;
        mStrCode = e.mStrCode;
    }
    return *this;
}

Exception::~Exception()
{
}

void Exception::defaultExceptionProcesser(Exception& e)
{
	fatalError("Exception catched: file:%s like:%d function:%s [%d] [%s] [%s]\n", e.file(), e.line(), e.func(), e.code(), e.codes(), e.what());
}

const char* aps::Exception::file()const
{
	return mSrcInfo.file();
}

int aps::Exception::line()const
{
	return mSrcInfo.line();
}

const char* aps::Exception::codes()const
{
	return mStrCode.c_str();
}

const char* aps::Exception::func()const
{
	return mSrcInfo.func();
}

int aps::Exception::code()const
{
	return mIntErrCode;
}

const char* aps::Exception::what()const
{
	return mErrMsg.c_str();
}

void aps::Exception::setCode(int icode)
{
	mIntErrCode = icode;
}

void aps::Exception::setWhat(const string& msg)
{
	mErrMsg = msg;
}

void aps::Exception::setSourceInfo( SourceInfo sourceInfo )
{
    mSrcInfo = sourceInfo;
}

//////////////////////////////////////////////////////////////////////////
void aps::logExceptionEx( SourceInfo sourceInfo, int iCode, const string& errMsg )
{
    LoggerAgent(sourceInfo, eERROR) << "Exception("<<iCode<<":"<<errMsg<<")";
    throw Exception(sourceInfo, iCode, errMsg);
}


void aps::logExceptionEx( SourceInfo sourceInfo, const string& sCode, const string& errMsg )
{
    LoggerAgent(sourceInfo, eERROR) << "Exception("<<sCode<<":"<<errMsg<<")";
    throw Exception(sourceInfo, sCode, errMsg);
}

void aps::logException( SourceInfo sourceInfo, int iCode, const char* fmt, ... )
{
	// 这是一段相对稳定标准的自适应变参处理代码, 基本不需要修改
    // --------------------勿修改开始 ----------------------------
	char *pBuffer = NULL;
	fmt = fmt == NULL?"":fmt;

	if(NULL == strchr(fmt,'%'))
	{
		int size = strlen(fmt) + 1;
		pBuffer = (char*)calloc(size,sizeof(char));
		if(NULL!=pBuffer)
		{
			memcpy(pBuffer,fmt,size);
			pBuffer[size - 1] = '\0';
		}
	}
	else
	{
		va_list args;
		char* pInternalBuffer = NULL;
		char* pRallocBuffer = NULL;
		size_t bufferSize = 0;
		size_t nexSize = 0;

		for(;;)
		{
			if(bufferSize == 0)
			{
				int _size = (int)(strlen(fmt)*1.5);
				if(_size < 512)
					_size = 512;
				pInternalBuffer = (char*)malloc(_size);
				if(pInternalBuffer == NULL)
				{
					//内存错误
					pInternalBuffer = NULL;
					break;
				}
				bufferSize = _size;
			}
			else if((pRallocBuffer = (char *)realloc(pInternalBuffer, nexSize)) != NULL)
			{
				pInternalBuffer = pRallocBuffer;
				bufferSize = nexSize;
			}
			else
			{
				pInternalBuffer = NULL;
				break;
			}
			va_start(args,fmt);
			int realSize = vsnprintf(pInternalBuffer,bufferSize,fmt,args);
			va_end(args);

			if(realSize == -1)
				nexSize = bufferSize * 2;//只能瞎猜大小
			else if(realSize == (int)bufferSize)
				nexSize = bufferSize * 2;// 被截断, 大小亦不知道
			else if(realSize > (int)bufferSize)
				nexSize = realSize + 2;// 这个还好, 已经知道需要多大内存
			else if(realSize == (int)bufferSize - 1)
				nexSize = bufferSize * 2;// 在用些系统上不明确
			else
				break;// 这样最好
		}
		if(pInternalBuffer != NULL)
			pBuffer = pInternalBuffer;
	}
	// --------------------勿修改结束 ----------------------------
	string errMsg = (pBuffer == NULL)?fmt:pBuffer;
	if(pBuffer != NULL)
	{
		free(pBuffer);
		pBuffer = NULL;
	}
	LoggerAgent(sourceInfo, eERROR) << errMsg;
    throw Exception(sourceInfo, iCode, errMsg);
}

void aps::logException( SourceInfo sourceInfo, const string& sCode, const char* fmt, ... )
{
	// 这是一段相对稳定标准的自适应变参处理代码, 基本不需要修改
    // --------------------勿修改开始 ----------------------------
	char *pBuffer = NULL;
    fmt = fmt==NULL?"":fmt;

    if( NULL == strchr( fmt, '%' ) )
    {
        int size = strlen(fmt)+1;
        pBuffer = (char*)calloc(size, sizeof(char));
        if (NULL!=pBuffer)
        {
            memcpy(pBuffer, fmt, size);
            pBuffer[size-1] = '\0';
        }
    }
    else
    {
        va_list args;
        char *pInternalBuffer = NULL;
        char *pRallocBuffer = NULL;
        size_t bufferSize = 0;
        size_t nexSize = 0;

        for (;;)
        {
            if (bufferSize == 0) 
            {
                int _size = (int) (strlen(fmt)*1.5);
                if (_size<512)
                    _size = 512;
                pInternalBuffer = (char *)malloc(_size);
                if (pInternalBuffer == NULL) 
                {
                    // 内存错误
                    pInternalBuffer = NULL;
                    break;
                }
                bufferSize = _size;
            } 
            else if ((pRallocBuffer = (char *)realloc(pInternalBuffer, nexSize)) != NULL) 
            {
                pInternalBuffer = pRallocBuffer;
                bufferSize = nexSize;
            } 
            else
            {
                pInternalBuffer = NULL;
                break;
            }

            va_start(args, fmt);
            int realSize = vsnprintf(pInternalBuffer, bufferSize, fmt, args);
            va_end(args);

            if (realSize == -1) 
                nexSize = bufferSize * 2;// 只能瞎猜大小 
            else if (realSize == (int)bufferSize)
                nexSize = bufferSize * 2;// 被截断, 大小亦不知道
            else if (realSize > (int)bufferSize) 
                nexSize = realSize + 2;// 这个还好, 已经知道需要多大内存
            else if (realSize == (int)bufferSize - 1)
                nexSize = bufferSize * 2;// 在用些系统上不明确
            else
                break;// 这样最好
        }

        if (pInternalBuffer!=NULL)
            pBuffer = pInternalBuffer;
    }
    // --------------------勿修改结束 ----------------------------

    string errMsg = (pBuffer==NULL)?fmt:pBuffer;
    if (pBuffer!=NULL)
    {
        free(pBuffer);
        pBuffer = NULL;
    }
    
    LoggerAgent(sourceInfo, eERROR) << errMsg;
    throw Exception(sourceInfo, sCode, errMsg);
}

void aps::logExceptionX( SourceInfo sourceInfo, int iCode, const string& errMsg )
{
    LoggerAgent(sourceInfo, eERROR, &getInternalLogManager()) << "Exception("<<iCode<<":"<<StrHelper::trim(errMsg)<<")";
    throw Exception(sourceInfo, iCode, errMsg);
}

void aps::logExceptionX( const char* lpFile, int iLine, const char*lpFunc, int iCode, const string& errMsg )
{
    LoggerAgent(SourceInfo(lpFile, iLine, lpFunc), eERROR, &getInternalLogManager()) << "Exception("<<iCode<<":"<<StrHelper::trim(errMsg)<<")";
    throw Exception(lpFile, iLine, lpFunc, iCode, errMsg);
}