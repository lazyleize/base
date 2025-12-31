/*
* @file    logger.cpp
* @brief   日志对象类
* @author  leize<leize@xgd.com>
*
*/

#include <base/logger.hpp>
#include <base/consoleLogger.hpp>
#include <base/membuffer.hpp>
#include <base/strHelper.hpp>
#include <base/toolkit.hpp>
#include <base/datetime.hpp>
#include <base/file.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

using namespace aps;


#define ___DBG fprintf(stderr, "> %s, %s:%03d\n", __FILE__, __FUNCTION__, __LINE__);
#undef ___DBG
#define ___DBG


Logger::Logger(string name,LogLevel_t level):mIsActive(false),mLogName(name),mLogLevel(level)
{
	mNameSeprator = ".";
    mIsNameWithDate = true;
    mIsBeginWithDate = true;
    mIsPrefixDateDir = true;
    mNameWithPID = false;
    
    mLogMaxSize = 50*1024*1024;
}

Logger::~Logger()
{}

LogLevel_t Logger::str2LogLevel(const string& str_)
{
	string str = StrHelper::toUpper(str_);
	if(str == "TRACE")
		return eTRACE;
	else if(str == "DEBUG")
        return eDEBUG;
    else if(str == "INFOR")
        return eINFO;
    else if(str == "NOTIC")
        return eNOTICE;
    else if(str == "WARNI")
        return eWARNING;
    else if(str == "ERROR")
        return eERROR;
    else if(str == "ERROR")
        return eCRITICAL;
    else if(str == "EMERG")
        return eEMERGENCY;
    return eINFO;
}

string Logger::logLevel2Str(LogLevel_t l)
{
    switch(l)
    {
    case eTRACE:     return "TRACE";
    case eDEBUG:     return "DEBUG";
    case eINFO:      return "INFOR";
    case eNOTICE:    return "NOTIC";
    case eWARNING:   return "WARNI";
    case eERROR:     return "ERROR";
    case eCRITICAL:  return "CRITI";
    case eEMERGENCY: return "EMERG";
    }
    return "UNKNO";
}

void Logger::setPrefixFormat(string prefixFormat)
{
	mPrefixFormat = prefixFormat;
}

string& Logger::getPrefixFormat()
{
    return mPrefixFormat;
}

string Logger::parseFormat(string& inFormat, const char*date_, const char*time_, int processID, int threadID,
                            const char*souceFile, int line, LogLevel_t l, const char*function_)
{
    Z_ASSERT (date_!=0 && time_!=0 && souceFile!=0 && function_!=0);

    // 日志前缀格式变量说明:
    //---------------------------------------------------
    // {Date}       日期
    // {Time}       时间
    // {DateTime}   日期+时间
    // {ProcessID}  进程ID(%5d)
    // {ThreadID}   线程ID(%5d)
    // {SourceFile} 源文件名(后5字节)
    // {SourceLine} 代码行(%4d)
    // {LogLevel}   日志级别(5字节)
    // {Function}   当前函数名Namespace::Class::Function

    if (inFormat.length()<=5)
        return inFormat;
    if (inFormat.find('{')==string::npos)
        return inFormat;
    if (inFormat.find("{D")==string::npos 
        && inFormat.find("{T")==string::npos
        && inFormat.find("{P")==string::npos
        && inFormat.find("{S")==string::npos
        && inFormat.find("{L")==string::npos 
        && inFormat.find("{F")==string::npos)
        return inFormat;

    static char szDate      [] = "{Date}";
    static char szTime      [] = "{Time}";
    static char szDateTime  [] = "{DateTime}";
    static char szProcessID [] = "{ProcessID}";
    static char szThreadID  [] = "{ThreadID}";
    static char szSourceFile[] = "{SourceFile}";
    static char szSourceLine[] = "{SourceLine}";
    static char szLogLevel  [] = "{LogLevel}";
    static char szFunction  [] = "{Function}";

    string outStr;
    outStr.reserve(200);
    string tmp;
    tmp.reserve(200);

    enum
    {
        eOut = 0,
        eInD,
        eInP,
        eInT,
        eInS,
        eInL,
        eInF
    };

    char c=0, lC=0;
    int state_ = eOut;
    char szBuffer[215] = {0};

    for (size_t i=0;i<inFormat.length();i++)
    {
        c = inFormat[i];
        switch (state_)
        {
        case eOut:
            {
                if (lC!='\\' && c=='{')
                {
                    int leftByte = inFormat.length()-i-1;
                    if ( leftByte <= 5 ) // {Date}
                    {
                        outStr += inFormat.c_str() + i;
                        return outStr;
                    }
                    switch (inFormat[i+1])
                    {
                    case 'D':
                        {
                            i++;
                            state_ = eInD;
                        }
                        break;
                    case 'T':
                        {
                            i++;
                            state_ = eInT;
                        }
                        break;
                    case 'P':
                        {
                            i++;
                            state_ = eInP;
                        }
                        break;
                    case 'L':
                        {
                            i++;
                            state_ = eInL;
                        }
                        break;
                    case 'S':
                        {
                            i++;
                            state_ = eInS;
                        }
                        break;
                    case 'F':
                        {
                            i++;
                            state_ = eInF;
                        }
                        break;
                    default:
                        ;
                    }
                }
                else
                {
                    outStr.append(1,c);
                }

                if (state_!=eOut)
                    lC = inFormat[i];
                else 
                    lC = c;
            }
            break;
        case eInD:
            {
                if (memcmp(inFormat.c_str()+i-2, szDate, sizeof(szDate)-1)==0)
                {
                    outStr += date_;
                    i += sizeof(szDate)-4;
                    lC = '}';
                }
                else if ( (i+sizeof(szDateTime)-2 <= inFormat.length()) && memcmp(inFormat.c_str()+i-2, szDateTime, sizeof(szDateTime)-1)==0)
                {
                    outStr += date_;
                    outStr += " ";
                    outStr += time_;
                    i += sizeof(szDateTime)-4;
                    lC = '}';
                }
                else
                {
                    lC = c;
                    outStr.append(szDateTime, 3);
                }
                state_ = eOut;
            }
            break;
        case eInP:
            {
                if (memcmp(inFormat.c_str()+i-2, szProcessID, sizeof(szProcessID)-1)==0)
                {
                    snprintf(szBuffer, sizeof(szBuffer), "%08d", processID);
                    outStr += szBuffer;
                    i += sizeof(szProcessID)-4;
                    lC = '}';
                }
                else
                {
                    lC = c;
                    outStr.append(szProcessID, 3);
                }
                state_ = eOut;
            }
            break;
        case eInT:
            {
                if (memcmp(inFormat.c_str()+i-2, szTime, sizeof(szTime)-1)==0)
                {
                    outStr += time_;
                    i += sizeof(szTime)-4;
                    lC = '}';
                }
                else if ( (i+sizeof(szThreadID)-2 <= inFormat.length()) && memcmp(inFormat.c_str()+i-2, szThreadID, sizeof(szThreadID)-1)==0)
                {
                    snprintf(szBuffer, sizeof(szBuffer), "%08d", threadID);
                    outStr += szBuffer;
                    i += sizeof(szThreadID)-4;
                    lC = '}';
                }
                else
                {
                    lC = c;
                    outStr.append(szThreadID, 3);
                }
                state_ = eOut;
            }
            break;
        case eInS:
            {
                if ( (i+sizeof(szSourceFile)-2 <= inFormat.length()) && memcmp(inFormat.c_str()+i-2, szSourceFile, sizeof(szSourceFile)-1)==0)
                {
                    if ((int)strlen(souceFile)>kMaxSourceFileName)
                        souceFile += strlen(souceFile) - kMaxSourceFileName;

                    snprintf(szBuffer, sizeof(szBuffer), "%*s", kMaxSourceFileName-4, souceFile);

                    outStr += szBuffer;
                    i += sizeof(szSourceFile)-4;
                    lC = '}';
                }
                else if ( (i+sizeof(szSourceLine)-2 <= inFormat.length()) && memcmp(inFormat.c_str()+i-2, szSourceLine, sizeof(szSourceLine)-1)==0)
                {
                    snprintf(szBuffer, sizeof(szBuffer), "%04d", line);
                    outStr += szBuffer;
                    i += sizeof(szSourceLine)-4;
                    lC = '}';
                }
                else
                {
                    lC = c;
                    outStr.append(szSourceFile, 3);
                }
                state_ = eOut;
            }
            break;
        case eInL:
            {
                if ( (i+sizeof(szLogLevel)-2 <= inFormat.length()) && memcmp(inFormat.c_str()+i-2, szLogLevel, sizeof(szLogLevel)-1)==0)
                {
                    outStr += logLevel2Str(l);
                    i += sizeof(szLogLevel)-4;
                    lC = '}';
                }
                else
                {
                    lC = c;
                    outStr.append(szLogLevel, 3);
                }
                state_ = eOut;
            }
            break;
        case eInF:
            {
                if ( (i+sizeof(szFunction)-2 <= inFormat.length()) && memcmp(inFormat.c_str()+i-2, szFunction, sizeof(szFunction)-1)==0)
                {
                    string f = function_?function_:"";
                    int startPos = f.length() - MAX_FUNCTION_LENGTH;
                    if (startPos<=0)
                        startPos = 0;
                    if (startPos>0)
                    {
                        f = f.substr(startPos);
                        f[0] = '*';
                    }
                    outStr += f;
                    i += sizeof(szFunction)-4;
                    lC = '}';
                }
                else
                {
                    lC = c;
                    outStr.append(szFunction, 3);
                }
                state_ = eOut;
            }
            break;
        default:
            ;
        }
    }

    return outStr;
}

string Logger::logBinary(const unsigned char *in, int len)
{
    MemBuffer buffer;

    int cur = 0;
    int row_cnt = 0;
    const int countPerRow = 20;
    int rightPrintedCount = 0;

#ifndef Z_OS_WIN32
    buffer.format("Binary, %d Bytes[\n%03d:", len, row_cnt++);
#else
	buffer.format("Binary, %d Bytes[\r\n%03d:", len, row_cnt++);
#endif
    do 
    {
        if (cur % countPerRow == 0 && cur > 0)
        {
            buffer.format(" | ");
            const unsigned char*p = in + cur -countPerRow;
            rightPrintedCount += countPerRow;
            for (int i=0; i<countPerRow; i++)
            {
                if (isgraph(p[i]))
                    buffer.format("%c", p[i]);
                else
                    buffer.format(".");
            }
#ifndef Z_OS_WIN32
            buffer.format("\n%03d:",countPerRow*row_cnt++);
#else
			buffer.format("\r\n%03d:",countPerRow*row_cnt++);
#endif
        } 
        buffer.format(" %02x",in[cur]);
        cur++;
    } while(cur < len);

    int leftCount = countPerRow - len%countPerRow;
    if (leftCount != countPerRow)
    {
        for (int i=0; i<leftCount; i++)
        {
            buffer.format("   ");
        }
    }

    leftCount = len - rightPrintedCount;
    if (leftCount != 0)
    {
        buffer.format(" | ");
        const unsigned char*pLeft = in + len -leftCount;
        for (int i=0; i<leftCount; i++)
        {
            if (isgraph(pLeft[i]) || pLeft[i] == 0x20) // 空格也要显示啊
                buffer.format("%c", pLeft[i]);
            else
                buffer.format(".");
        }
    }
#ifndef Z_OS_WIN32
    buffer.format("\n]\n");
#else
	buffer.format("\r\n]\r\n");
#endif
    return buffer.getBuffer();
}

void Logger::dumpBinary(SourceInfo si, LogLevel_t level, const unsigned char *in, int len)
{
    log(si, level, logBinary(in, len).c_str());
}


void Logger::setMaxSize(int maxSize)
{
    // 大于100MB的日志已基本无法阅读
    if (maxSize>0 && maxSize<=100*1024*1024)
    {
        mLogMaxSize = maxSize;
    }
}


int Logger::getMaxSize()
{
    return mLogMaxSize;
}


void Logger::setFileNameWithDate( bool bNameWithDate_, string dateFormat, string strSeprator/*="."*/, bool bBeginWithDate_/*=true*/ )
{
    mIsNameWithDate = bNameWithDate_;
    mIsBeginWithDate = bBeginWithDate_;
    mNameSeprator = strSeprator;
    mDateFormat = dateFormat;
}


// 如果当前文件大小超出最大文件
void archiveLogFile(int maxSize, string sep, string curFile)
{
    string curFileNoSuffix = curFile;

    // 取没有后缀的名字
    if (FileInfo::getSuffix(curFileNoSuffix)=="log")
        curFileNoSuffix = curFileNoSuffix.substr(0, curFileNoSuffix.length()-4);

    string curFile_ = curFileNoSuffix + ".log";
    if (File::exists(curFile_))
    {
        FileInfo fi(curFile_);
        off_t fileSize = fi.size();
      
        if (fileSize>=maxSize)
        {
            string nextFile;
            int nextFileIndex = 0;
            
            for(; ;nextFileIndex++)
            {
                // 取 curFile + sep + N + .log
                nextFile = curFileNoSuffix + sep + formatStr("%02d", nextFileIndex).str() + ".log";
                if (File::exists(nextFile))
                    continue;
                else 
                    break;
            }
            
            rename(curFile_.c_str(), nextFile.c_str());                        
        }
    }
}

std::string Logger::getLogFileFullPath()
{
    string path;

    // 目录
    path = mLogFileDir;
    if (path=="")
        path = ".";

    if (path[path.length()-1]!='/' && path[path.length()-1]!='\\')
#ifndef Z_OS_WIN32
         path += "/";
#else
		 path += "\\";
#endif

    Date thisDay = Date::now();

    // 是否需建立以日期为名的目录 
    if (mIsPrefixDateDir)
    {
#ifndef Z_OS_WIN32
        path += thisDay.format(mPrefixDateDirFormat/*"YM"*/) + "/";
#else
		path += thisDay.format(mPrefixDateDirFormat/*"YM"*/) + "\\";
#endif
        Directory::mkdir(path);
    }
	else
		Directory::mkdir(path);

    // 前缀
    if (mIsNameWithDate && mIsBeginWithDate)
    {
        if (mDateFormat=="")
            path += thisDay.format("YMD");
        else 
            path += thisDay.format(mDateFormat);
        
        path += mNameSeprator;
    }

    path += mLogName;

    // 后缀
    if (mIsNameWithDate && !mIsBeginWithDate)
    {
        path += mNameSeprator;

        if (mDateFormat=="")
            path += thisDay.toCStr();
        else 
            path += thisDay.format(mDateFormat);
    }

    // 进程号作为日志名
    if (mNameWithPID)
    {
        path += mNameSeprator;
        char buffer[15] = {0};
        snprintf(buffer, sizeof(buffer), "%08d", Toolkit::getCurrentProcessId());
        path += buffer;
    }

    // 如果日志大小超出mLogMaxSize则将其移为归档日志
    if (mLogMaxSize>0)
    {
        archiveLogFile(mLogMaxSize, mNameSeprator, path);
    }

    if (FileInfo::getSuffix(path)!="log")
        path += ".log";
    return path;
}



void Logger::setPrefixDateDir( bool v , string dateFormat)
{
    mIsPrefixDateDir = v;
    mPrefixDateDirFormat = dateFormat;
}

ProfileTracer::ProfileTracer(const SourceInfo& si, LogLevel_t level_, const char*pName/*=NULL*/):
m_si(si),
level(level_)
{
    m_name = pName == NULL?m_si.func():pName;
    gettimeofday( &m_start, NULL );
    LoggerAgent(m_si, level) << "Enter " << m_name;
}


ProfileTracer::~ProfileTracer()
{
    struct timeval end;
    gettimeofday( &end, NULL );
    double duration = 0;

    duration = 1000000 * ( end.tv_sec - m_start.tv_sec ) + end.tv_usec - m_start.tv_usec; 
    duration /= 1000000;
    
    char szTmp[100]="";
    snprintf(szTmp, 
        sizeof(szTmp), "Leave %s, cost %0.6lf seconds", 
        m_name.c_str(), 
        duration);
    
    LoggerAgent(m_si, level) <<szTmp;
}


void aps::fatalError( const char*fmt ... )
{
    char buffer[1024] = {0};
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    buffer[sizeof(buffer)-1] = 0;
    
    FILE *fp = fopen("error.txt", "ab");
    if (fp)
    {
        Datetime now = Datetime::now();
        fprintf(fp, "%s %s", now.toCStr(), buffer);
        fprintf(stderr, "%s %s", now.toCStr(), buffer);
        fclose(fp);
    }
    va_end(args);
}



LogManager& aps::getInternalLogManager()
{
    static Mutex mutex;
    ScopedLock<Mutex> lock(mutex);
    static LogManager* pMan = NULL;
    if (pMan==NULL)
        pMan = new LogManager;
    return *pMan;
}


aps::LoggerAgent::LoggerAgent( SourceInfo si, LogLevel_t level, Logger*pLoggerTo/*=NULL*/ ):
mFlushAtEnd(true)
{
    si_ = si;
    level_ = level;
    pLoggerTo_ = pLoggerTo;
    pLogMan_ = NULL;
}

aps::LoggerAgent::LoggerAgent( SourceInfo si, LogLevel_t level, LogManager*pManager ):
mFlushAtEnd(true)
{
    si_ = si;
    level_ = level;
    pLoggerTo_ = NULL;
    pLogMan_ = pManager;
}

aps::LoggerAgent::~LoggerAgent()
{
    if(mFlushAtEnd)
        log_(si_, level_, oss.str().c_str());
}

void aps::LoggerAgent::log( SourceInfo sourceInfo, LogLevel_t level, const char*fmt ... )
{
    mFlushAtEnd = false;
    char *mBuffer = NULL;
    fmt = fmt==NULL?"":fmt;
    
    if( NULL == strchr( fmt, '%' ) )
    {
        int size = strlen(fmt)+1;
        mBuffer = (char*)calloc(size, sizeof(char));
        if (NULL!=mBuffer)
        {
            memcpy(mBuffer, fmt, size);
            mBuffer[size-1] = '\0';
        }
    }
    else
    {
        va_list args;
        char *pBuffer = NULL;
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
                pBuffer = (char *)malloc(_size);
                if (pBuffer == NULL) 
                {
                    // 内存错误
                    pBuffer = NULL;
                    break;
                }
                bufferSize = _size;
            } 
            else if ((pRallocBuffer = (char *)realloc(pBuffer, nexSize)) != NULL) 
            {
                pBuffer = pRallocBuffer;
                bufferSize = nexSize;
            } 
            else
            {
                pBuffer = NULL;
                break;
            }

            va_start(args, fmt);
            int realSize = vsnprintf(pBuffer, bufferSize, fmt, args);
            va_end(args);

            if (realSize == -1)
            {
                // 只能瞎猜大小 
                nexSize = bufferSize * 2;
            } 
            else if (realSize == (int)bufferSize)
            {
                // 被截断, 大小亦不知道
                nexSize = bufferSize * 2;
            } 
            else if (realSize > (int)bufferSize) 
            {
                // 这个还好, 已经知道需要多大内存
                nexSize = realSize + 2;
            } 
            else if (realSize == (int)bufferSize - 1)
            {
                // 在用些系统上不明确(不明白...)
                nexSize = bufferSize * 2;
            } 
            else
            {
                // 这样最好
                break;
            }
        }

        if (pBuffer!=NULL)
            mBuffer = pBuffer;
    }

    if (mBuffer!=NULL)
    {
        log_(sourceInfo, level, mBuffer);
        free(mBuffer);
        mBuffer = NULL;
    }
}

void aps::LoggerAgent::log_( SourceInfo sourceInfo, LogLevel_t level, const char*szLog )
{
    if (pLoggerTo_==NULL)
        if (pLogMan_==NULL)
            LogManager::instance().log(sourceInfo, level, szLog);
        else 
            pLogMan_->log(sourceInfo, level, szLog);
    else 
        pLoggerTo_->log(sourceInfo, level, szLog);
}


void aps::Logger::setLogLevel( LogLevel_t l )
{
    mLogLevel = l;
}

aps::LogLevel_t aps::Logger::logLevel() const
{
    return mLogLevel;
}

std::string aps::Logger::name()
{
    return mLogName;
}

std::string aps::Logger::path()
{
    return mLogFileDir;
}

void aps::Logger::setName(string name)
{
    mLogName = name;
}

void aps::Logger::setPath(string path)
{
    mLogFileDir = path;
}

void aps::Logger::setNameWithPID(bool v)
{
    mNameWithPID = v;
}

bool aps::Logger::getNameWithPID()
{
    return mNameWithPID;
}

LogManager::LogManager()
{
    mDefaultLogger = new ConsoleLogger(LIBAPS_DEFAULT_LOGGER_NAME, eINFO);
    mDefaultLogger->start("stderr");
    mLoggerMap.insert( make_pair(LIBAPS_DEFAULT_LOGGER_NAME, mDefaultLogger) );
}

LogManager::~LogManager()
{
    {
        Mutex::ScopedLock gard(mLock);

        ___DBG;
        auto iter = mLoggerMap.begin();
        for (;iter!=mLoggerMap.end();iter = mLoggerMap.begin())
        {
            Logger*pLogger = iter->second;

            if (pLogger!=NULL)
            {
                ___DBG;
                delete pLogger;
            }

            if (pLogger==mDefaultLogger)
                mDefaultLogger = NULL;

            mLoggerMap.erase(iter);
        }
        mLoggerMap.clear();

        if (mDefaultLogger!=NULL)
        {
            ___DBG;
            delete mDefaultLogger;
            mDefaultLogger = NULL;
        }
    }
}

void LogManager::dumpBinary(SourceInfo si, LogLevel_t level, const unsigned char *in_, int len)
{
    Mutex::ScopedLock gard(mLock);
    static MemBuffer buffer;
    const int kBufferSize = 1024*200;
    buffer.more(kBufferSize);
    buffer.reset();

    auto iter = mLoggerMap.begin();
    for (;iter!=mLoggerMap.end();iter++)
    {
        Logger*pLogger = iter->second;
        if (pLogger!=NULL)
        {
            if (pLogger->isActive())
            {    
                pLogger->dumpBinary(si, level, in_, len);
            }
        }
    }
}

void LogManager::log(SourceInfo sourceInfo, LogLevel_t level, const char*szLog)
{
    Mutex::ScopedLock gard(mLock);

    auto iter = mLoggerMap.begin();
    for (;iter!=mLoggerMap.end();iter++)
    {
        Logger*pLogger = iter->second;
        if (pLogger!=NULL)
        {
            if (pLogger->isActive())
            {    
                pLogger->log(sourceInfo, level, szLog);
            }
        }
    }
}


void LogManager::setDefaultLogger(Logger*pLogger)
{
    Mutex::ScopedLock gard(mLock);
    if (pLogger!=NULL)
    {
        if (mDefaultLogger!=pLogger)
        {
            if (mDefaultLogger!=NULL)
            {
                mDefaultLogger->stop();
                map<string, Logger*>::iterator i = mLoggerMap.find(mDefaultLogger->name());
                if(i != mLoggerMap.end())
                    mLoggerMap.erase(i);
                delete mDefaultLogger;
                mDefaultLogger = NULL;
            }

            mDefaultLogger = pLogger;
            if (pLogger!=NULL)
                mLoggerMap.insert( make_pair(LIBAPS_DEFAULT_LOGGER_NAME, mDefaultLogger) );
        }
    }
}


Logger * LogManager::getDefaultLogger()
{
    return mDefaultLogger;
}


void LogManager::registerLogger(Logger*pNewLogger)
{
    if (pNewLogger==NULL)
        return;

    Mutex::ScopedLock gard(mLock);

    map<string, Logger*>::iterator i = mLoggerMap.find(pNewLogger->name());
    if(i != mLoggerMap.end())
        return ;

    for( auto iter = mLoggerMap.begin(); iter != mLoggerMap.end(); ++iter )
    {
        if (iter->second==pNewLogger)
            return ;
    }

    mLoggerMap.insert( make_pair(pNewLogger->name(), pNewLogger) );
}


Logger* LogManager::getLogger(string name)
{
    Mutex::ScopedLock gard(mLock);

    map<string, Logger*>::iterator i = mLoggerMap.find(name);
    if(i != mLoggerMap.end())
        return i->second;
    return NULL;
}


LogManager& LogManager::instance()
{
    static LogManager manager;
    return manager;
}
