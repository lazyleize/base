/*
 * @file    consoleLogger.cpp
 * @brief   日志打印到终端
 * @author  leize<leize@xgd.com>
 *
 * @copyright   xgd
*/
#include <base/consoleLogger.hpp>
#include <base/strHelper.hpp>
#include <base/membuffer.hpp>
#include <base/datetime.hpp>
#include <base/file.hpp>
#include <base/toolkit.hpp>
#include <base/thread.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

using namespace aps;

ConsoleLogger::ConsoleLogger(string name, LogLevel_t level)
    :Logger(name, level), mFileFd(stderr)
{
    setPrefixFormat(Z_DEFAULT_LOG_PREFIX_FORMAT_SIMPLE);
}

ConsoleLogger::~ConsoleLogger()
{
}

void ConsoleLogger::start(const std::string& path)
{
    if (StrHelper::toLower(path)=="stderr")
        mFileFd = stderr;
    else if (StrHelper::toLower(path)=="stdout")
        mFileFd = stdout;
    else
        mFileFd = stderr;

    mIsActive = true;
}

void ConsoleLogger::stop()
{
    mIsActive = false;
}

void ConsoleLogger::restart()
{
    mIsActive = true;
}

bool ConsoleLogger::isActive()
{
    return mIsActive;
}

void ConsoleLogger::log(SourceInfo sourceInfo, LogLevel_t level, const char*szLog)
{
    if (!isActive())
        return;

    if (mLogLevel>level)
        return;
    
    FileInfo fileInfo(sourceInfo.file());

    Datetime tmNow = Datetime::now();
    string dtStr = tmNow.getDate().toString();
    string tmStr = tmNow.getTime().format("H:I:S.U");
    
    string toWrite = parseFormat(mPrefixFormat, dtStr.c_str(), tmStr.c_str(), 
        Toolkit::getCurrentProcessId(), Thread::getCurrentThreadId(), fileInfo.name().c_str(), 
        sourceInfo.line(), level, sourceInfo.func());

    toWrite += szLog ? szLog:"";
    if (toWrite.length()>1 && toWrite[toWrite.length()-1]!='\n')
        toWrite += '\n';

    fprintf(mFileFd, "%s", toWrite.c_str());
}



//////////////////////////////////////////////////////////
#ifdef Z_OS_WIN32
LoggerBindColor::LoggerBindColor(string name,LogLevel_t level):ConsoleLogger(name,level)
{

	setPrefixFormat(Z_DEFAULT_LOG_PREFIX_FORMAT_SIMPLE);
}

LoggerBindColor::~LoggerBindColor()
{
}

void LoggerBindColor::setColorScheme(ColorPlan cp,eConsoleColor bo)
{
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	eConsoleColor f;
	//if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbiInfo)) 
	switch(cp)
	{
	case 0: f = eConColorWhite;break;
	case 1: f = eConColorGray;break;
	case 2: f = eConColorGreen; break;
	case 3: f = eConColorAqua; break;
	case 4: f = eConColorRed; break;
	case 5: f = eConColorPurple;break;
	case 6: f = eConColorYellow;break;
	case 7: f = eConColorLightBlue; break;
	case 8: f = eConColorLightGreen; break;
	case 9: f = eConColorLightAqua; break;
	case 10:f = eConColorLightRed; break;
	case 11:f = eConColorLightPurple; break;
	case 12:f = eConColorLightYellow; break;
	case 13:f = eConColorBrightWhite; break;


	default:f = eConColorWhite;break;
	}
	//保留原本的颜色方案
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbiInfo))
	{
		oldf = eConColorWhite;
		oldb = eConColorBlack;
	}
	else
	{
		oldf = (eConsoleColor) (csbiInfo.wAttributes % 16);
		oldb = (eConsoleColor) (csbiInfo.wAttributes / 16);
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bo*16+f);
}

void LoggerBindColor::severtColor()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),oldb*16+oldf);
}

void LoggerBindColor::defColor()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),eConColorWhite);
}

void LoggerBindColor::printf_colo(SourceInfo sourceInfo, LogLevel_t level, const char*szLog)
{
	
	FileInfo fileInfo(sourceInfo.file());

	Datetime tmNow = Datetime::now();
	string dtStr = tmNow.getDate().toString();
	string tmStr = tmNow.getTime().format("H:I:S.U");

	string toWrite = parseFormat(mPrefixFormat, dtStr.c_str(), tmStr.c_str(), 
		Toolkit::getCurrentProcessId(), Thread::getCurrentThreadId(), fileInfo.name().c_str(), 
		sourceInfo.line(), level, sourceInfo.func());

	toWrite += szLog ? szLog:"";
	if (toWrite.length()>1 && toWrite[toWrite.length()-1]!='\n')
		toWrite += '\n';

	fprintf(stdout, "%s", toWrite.c_str());
}


#endif

//////////////////////////////////////////////////////