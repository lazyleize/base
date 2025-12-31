/*
 * @file        sourceInfo.cpp
 * @brief       代码所在行信息,用于日志和异常类
 * @author      leize<leize@xgd.com>
 *
 * @copyright   xgd
*/
#include <base/sourceInfo.hpp>
#include <base/toolkit.hpp>
#include <string.h>
#include <stdio.h>

using namespace aps;

SourceInfo::SourceInfo()throw()
{
	mLine = 0;
	memset(mSzFunc,0x00,sizeof(mSzFunc));
	memset(mSzFile,0x00,sizeof(mSzFile));
}

SourceInfo::SourceInfo(const SourceInfo& si)throw()
{
	mLine = si.mLine;
	snprintf(mSzFile,sizeof(mSzFile),"%s",si.mSzFile);
	snprintf(mSzFunc,sizeof(mSzFunc),"%s",si.mSzFunc);
}

SourceInfo::SourceInfo(const char* _file,unsigned int _line,const char* _func)throw()
{
	_file = _file!=NULL?_file:"";
	_func = _func!=NULL?_func:"";

	mLine = _line;
	snprintf(mSzFile,sizeof(mSzFile),"%s",_file);
	snprintf(mSzFunc,sizeof(mSzFunc),"%s",_func);
}

SourceInfo& SourceInfo::operator=(const SourceInfo& si)
{
	if(&si != this)
	{
		memcpy(mSzFile,si.mSzFile,sizeof(mSzFile));
		memcpy(mSzFunc,si.mSzFunc,sizeof(mSzFunc));
		mLine = si.mLine;
	}
	return *this;
}

const char* aps::SourceInfo::file()const throw()
{
	return mSzFile;
}

unsigned int aps::SourceInfo::line()const throw()
{
	return mLine;
}

const char* aps::SourceInfo::func()const throw()
{
	return mSzFunc;
}