/*
* @file    consoleApplication.hpp
* @brief   终端程序基类
* @author  sunu<pansunyou@gmail.com>
* @copyright Copyright (c) 2015 xgd
*/
#include <base/consoleApplication.hpp>
#include <base/strHelper.hpp>
#include <base/fileLogger.hpp>
#include <file.hpp>

#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace aps;
using namespace std;

ApplicationArgs::ApplicationArgs():mArgv(NULL)
{
    mArgc = 0;
    mArgv = NULL;
}

ApplicationArgs::~ApplicationArgs()
{
    {
        ScopedLock<Mutex> lock(mMutex);
        if (mArgv!=NULL)
        {
            for (int i=0; i<mArgc; i++)
                free(mArgv[i]);

            delete[] mArgv;
            mArgv = NULL;
        }
    }
}

ApplicationArgs &ApplicationArgs::globalVal()
{
	static ApplicationArgs args;
	return args;
}

void ApplicationArgs::parseArgs(int argc,char**argv)
{
	ScopedLock<Mutex> lock(mMutex);

    if (mArgv!=NULL)
    {
        for (int i=0; i<mArgc; i++)
            free(mArgv[i]);

        delete[] mArgv;
        mArgv = NULL;
    }

    mArgc = argc;
    if (mArgc>0)
        mArgv = new char*[mArgc];
    
    for (int i=0; i<mArgc; i++)
        mArgv[i] = strdup(argv[i]);

    mArgMap.clear();
    string key,val,buffer;
    char tmpStr[256];
    int i=0;
    for (i=0; i<argc; i++)
    {
        if (argv[i]==NULL)
            continue;

        if (strlen(argv[i])==1 && argv[i][0]=='-')
            continue;

        if (strlen(argv[i])==2 && 0==strcmp(argv[i],"--"))
            continue;

        buffer = argv[i];
        if ( buffer[0]=='-')
        {
            key = argv[i];
            if ( (i+1)<argc ) 
            {
                // -nabc
                if (argv[i+1]!=NULL && argv[i+1][0]!='-' ) 
                {
                    val = argv[i+1];
                    if ( strlen(argv[i])>=3 && argv[i][1]!='-')
                    {
                        val = argv[i] + 2;
                        memset(tmpStr, 0x00, sizeof(tmpStr));
                        strncpy(tmpStr, argv[i], 2);
                        key = tmpStr;
                    }
                    mArgMap.insert( pair<string,string>(key, val) );
                    i++;
                }

                // --abc --***
                else if (argv[i+1][0]=='-')
                {
                    val = "";
                    if ( strlen(argv[i])>=3 && argv[i][1]!='-')
                    {
                        val = argv[i] + 2;
                        memset(tmpStr, 0x00, sizeof(tmpStr));
                        strncpy(tmpStr, argv[i], 2);
                        key = tmpStr;
                    }
                    mArgMap.insert( pair<string,string>(key, val) );
                }
            }
            else 
            {
                //-npd -c
                //--cdabc
                val = "";
                if ( strlen(argv[i])>=3 && argv[i][1]!='-')
                {
                    val = argv[i] + 2;
                    memset(tmpStr, 0x00, sizeof(tmpStr));
                    strncpy(tmpStr, argv[i], 2);
                    key = tmpStr;
                }
                mArgMap.insert( pair<string,string>(key, val) );
            }
        }
        else
        {
            mArgMap.insert( pair<string,string>(argv[i], "") );
        }
    }
}

string ApplicationArgs::getopt(string arg, string defVal)const
{
	return readArgString(arg,defVal);
}

bool ApplicationArgs::checkopt(string arg)const
{
	ScopedLock<Mutex>lock(mMutex);

	if(mArgMap.size()<=0)
		return false;
	for(multimap<string, string>::const_iterator iter = mArgMap.begin(); iter != mArgMap.end(); ++iter )
	{
		if(iter->first == arg)
			return true;
	}
	return false;
}

void ApplicationArgs::copyArgs(multimap<std::string, std::string>&toList)const
{
	ScopedLock<Mutex> lock(mMutex);

	for( std::multimap<std::string, std::string>::const_iterator iter = mArgMap.begin(); iter != mArgMap.end(); ++iter ) 
    {
        toList.insert( std::pair<std::string,std::string>(iter->first, iter->second) );
    }
}

void ApplicationArgs::dump()const 
{
    ScopedLock<Mutex> lock(mMutex);

    printf("Arguments\r\n------\r\n");
    for( multimap<string, string>::const_iterator iter = mArgMap.begin(); iter != mArgMap.end(); ++iter ) 
    {
        printf("%15s = [%s]\r\n", iter->first.c_str(), iter->second.c_str());
    }
}

int ApplicationArgs::readArgInt( string arg, int defVal/*=0*/ )const 
{
	if(!checkopt(arg))
		return defVal;
	return atoi(readArgString(arg,"0").c_str());
}

bool ApplicationArgs::readArgBool(string arg,bool defVal/*=false*/ )const 
{
	if(!checkopt(arg))
		return defVal;
	string Val = StrHelper::trim(readArgString(arg,"false"));
	Val = StrHelper::toLower(Val);
	if(Val=="false"||Val=="no"||Val==""||Val=="0")
		return false;
	else
		return true;
}

double ApplicationArgs::readArgDouble(string arg, double defVal/*=0*/ )const 
{
	if(!checkopt(arg))
		return defVal;
	return atof(readArgString(arg,"0").c_str());
}

string ApplicationArgs::readArgString(string arg, string defVal/*=""*/ )const
{
	ScopedLock<Mutex> lock(mMutex);

    if (mArgMap.size()<=0)
        return defVal;
    for( multimap<string, string>::const_iterator iter = mArgMap.begin(); iter != mArgMap.end(); ++iter ) 
    {
        if (iter->first == arg){
            return iter->second;
        }
    }
    return defVal;
}

TInt64 ApplicationArgs::readArgInt64( string arg, TInt64 defVal/*=0*/ )const 
{
    if (!checkopt(arg))
        return defVal;
    return atoi(readArgString(arg, "0").c_str());
}


//////////////////////////////////////////////////////////////////////////

ConsoleApplication::ConsoleApplication(int argc, char**argv)
{
	parseArgs(argc,argv);
}

int ConsoleApplication::run()
{
	return 0;
}

string ConsoleApplication::getAppFullname()
{
	string name = "unknown";
	try
	{
		const char* pName = mArgc>0?mArgv[0]:"unknown";
		FileInfo fi(pName);
		name = fi.name();
	}
	catch(...)
	{}
	return name;
}

string ConsoleApplication::getAppName()
{
    string name = "unknown";
    try
    {
        const char*pName = mArgc>0?mArgv[0]:"unknown";
        FileInfo fi(pName);
        name = fi.pureName();
    }
    catch (...)
    {}
    return name;
}

string ConsoleApplication::getAppPath()
{
    string path = ".";
    try
    {
        const char*pName = mArgc>0?mArgv[0]:".";
        FileInfo fi(pName);
        path = fi.path();
    }
    catch (...)
    {}
    return path;
}

//! 初始化日志
void ConsoleApplication::setupLogger(string LogName, int Debug, LogLevel_t LogLevel, string FilePath, int maxSize)
{
	// 默认对象一般是打印屏幕, 暂时关闭
    // Trace(eINFO, _G2U("日志选项设置"));
	LogManager::instance().getDefaultLogger()->stop();
	getInternalLogManager().getDefaultLogger()->stop();

	Logger* pLogger = LogManager::instance().getLogger(LogName);
	{
		if(pLogger==NULL)
			pLogger = new FileLogger(LogName,LogLevel);
		pLogger->start(FilePath);
		pLogger->setMaxSize(maxSize);
		pLogger->setLogLevel(LogLevel);
		pLogger->setNameWithPID(true);
		LogManager::instance().registerLogger(pLogger);
	}

	if(Debug == 1)
	{
		//Trace(eINFO, _G2U("开启调试模式"));
        pLogger = getInternalLogManager().getLogger(LogName);
        if (pLogger!=NULL)
        {
            LogName += "-DEBUG";
            pLogger->setName(LogName);
            pLogger->setMaxSize(maxSize);
            pLogger->setLogLevel(LogLevel);
        }
		else 
        {
            LogName += "-DEBUG";
            pLogger = getInternalLogManager().getLogger(LogName);
            if (pLogger==NULL)
                pLogger = new FileLogger(LogName, LogLevel);
            pLogger->start(FilePath);
            pLogger->setMaxSize(maxSize);
            pLogger->setLogLevel(eTRACE);
            //pLogger->setNameWithPID(false);
            getInternalLogManager().registerLogger(pLogger);
        }
	}
	//Trace(eINFO, _G2U("日志选项设置结束"));
}