/*
* @file    fileLogger.cpp
* @brief   日志打印到文件
* @author  leize<leize@xgd.com>
*
* @copyright   xgd
*/
#include <base/fileLogger.hpp>
#include <base/strHelper.hpp>
#include <base/file.hpp>
#include <base/toolkit.hpp>
#include <base/thread.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

using namespace aps;

FileLogger::FileLogger(string name,LogLevel_t level):Logger(name,level)
{
	setPrefixFormat(Z_DEFAULT_LOG_PREFIX_FORMAT);
}

FileLogger::~FileLogger()
{
	stop();
}

void FileLogger::start(const std::string& logFileDir)
{
	if(mIsActive)
		return ;

	string::size_type nBeg;
	if(mLogFileDir == "")
#ifndef Z_OS_WIN32
		mLogFileDir = "./";
#else
		mLogFileDir = "\\";
#endif

	nBeg = mLogFileDir.find("%");
	if(nBeg != string::npos)
	{
		setPrefixDateDir(true);
		mLogFileDir = logFileDir.substr(0,logFileDir.length()-nBeg);
		mLogFileFullpath = getLogFileFullPath();
		mIsActive = true;
		return;
	}
	mLogFileDir = logFileDir;
	mLogFileFullpath = getLogFileFullPath();
	mIsActive = true;
}

void FileLogger::stop()
{
	mIsActive = false;
}

void FileLogger::restart()
{
	stop();
	start(mLogFileDir);
}

bool FileLogger::isActive()
{
	return mIsActive;
}

void FileLogger::log(SourceInfo sourceInfo, LogLevel_t level, const char*szLog)
{
	if (!isActive())
        return;

    if (mLogLevel>level)
        return;

    if (mLogFileFullpath!=getLogFileFullPath())
    {
        restart();
    }

    FileInfo fileInfo(sourceInfo.file());
    
    Datetime tmNow = Datetime::now();
    string dtStr = tmNow.getDate().toString();
    string tmStr = tmNow.getTime().format("H:I:S.U");
    
    string toWrite = parseFormat(mPrefixFormat, dtStr.c_str(), tmStr.c_str(), 
        Toolkit::getCurrentProcessId(), Thread::getCurrentThreadId(), fileInfo.name().c_str(), 
        sourceInfo.line(), level, sourceInfo.func());
    
    toWrite += szLog ? szLog:"";
    if (toWrite.length()>1 && toWrite[toWrite.length()-1]!='\n')
#ifdef Z_OS_WIN32
        toWrite += "\r\n";
#else
		toWrite += "\n";
#endif
    
    try
    {
        FILE *fp = fopen(mLogFileFullpath.c_str(), "ab");
        if (fp)
        {
            fprintf(fp, "%s", toWrite.c_str());
            fclose(fp);
        }
    }
    catch (...)
    {}

}