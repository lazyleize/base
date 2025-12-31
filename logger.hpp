/*
* @file        logger.hpp
* @brief       日志基类
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_LOGGER_HPP__
#define __LIBAPS_LOGGER_HPP__

#include <base/config_.hpp>
#include <base/platform.hpp>
#include <base/toolkit.hpp>
#include <base/mutex.hpp>
#include <base/strHelper.hpp>
#include <base/datetime.hpp>
#include <string>
#include <sstream>
#include <map>


#ifdef WIN32
# pragma warning (disable:4251)
#endif 

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

#ifndef Z_DEFAULT_LOG_PREFIX_FORMAT
#define Z_DEFAULT_LOG_PREFIX_FORMAT "[{DateTime}][{ProcessID}:{ThreadID}][{SourceFile}:{SourceLine}][{LogLevel}] " 
#endif 

#ifndef Z_DEFAULT_LOG_PREFIX_FORMAT_SIMPLE
#define Z_DEFAULT_LOG_PREFIX_FORMAT_SIMPLE "[{Time}][{ProcessID}:{ThreadID}][{SourceFile}:{SourceLine}][{LogLevel}] " 
#endif 

#define LIBAPS_DEFAULT_LOGGER_NAME "libaps"
#define MAX_FUNCTION_LENGTH 30

const int kMaxSourceFileName = 24;

namespace aps
{
	using namespace std;

	//! 日志等级
	// !!!取值不得修改!!! 
    /// 若日志插件使用不同的日志级别, 请在
    /// 插件内部做适配.
    enum LogLevel_t 
    {
        eTRACE  = 0,    /*!< 内部调试信息 */
        eDEBUG  = 1,    /*!< 调试信息 */
        eINFO   = 2,    /*!< 普通记录 */
        eNOTICE = 3,    /*!< 提示信息 */
        eWARNING= 4,    /*!< 警告 */
        eERROR  = 5,    /*!< 错误 */
        eCRITICAL= 6,   /*!< 严重错误 */
        eEMERGENCY      /*!< 致命错误无法恢复 */
    };

	//! 日志对象基类
	class LIBAPS_API Logger
	{
	public:
		Logger(string name,LogLevel_t level);
		virtual ~Logger();

		//! 日志等级
		void setLogLevel(LogLevel_t l);
		LogLevel_t logLevel()const;
		static string logLevel2Str(LogLevel_t l);
        static LogLevel_t str2LogLevel(const string& str);

		//! 日志文件名
		string name();
        string path();
        void setName(string name);
        void setPath(string path);

		//! 是否启用
        virtual void start(const string& path) = 0;
        virtual void stop() = 0;
        virtual void restart() = 0;
        virtual bool isActive() = 0;

		//! 记录日志
        virtual void log(SourceInfo sourceInfo, LogLevel_t level, const char*szLog) = 0;

        //! 记录二进制数据
        void    dumpBinary(SourceInfo si, LogLevel_t level, const unsigned char *in, int len);

        /// 配置选项
        //////////////////////////////////////////////////////////////////////////
        string  getLogFileFullPath();

        //! 每行日志前缀
        void setPrefixFormat(string prefixFormat);
        string& getPrefixFormat();

        //! 是否启用$logDir/20120102/模式
        void  setPrefixDateDir(bool v, string dateFormat="YMD");

        //! 配置文件名加日期前缀
        void setFileNameWithDate(bool bNameWithDate_, string dateFormat="YMD", string strSeprator=".", bool bBeginWithDate_=true);
        
        //! 打印二进制数据
        static string logBinary(const unsigned char *in, int len);

        //! 格式化日志行前缀
        string  parseFormat(string& inFormat, const char*date_, 
            const char*time_, int processID, int threadID,
            const char*souceFile, int line, LogLevel_t l, 
            const char*function_);

        //! 日志最大体积
        void setMaxSize(int maxSize);
        int getMaxSize();

        //! 进程与日志绑定
        void setNameWithPID(bool v);
        bool getNameWithPID();

    protected:

        string  mPrefixFormat;//日志前缀
        bool    mIsPrefixDateDir;
        string  mPrefixDateDirFormat;

        bool    mIsActive;
        string  mLogName;
        string  mLogFileDir;
        LogLevel_t mLogLevel;

        string  mNameSeprator;
        string  mDateFormat;
        bool    mIsNameWithDate;
        bool    mIsBeginWithDate;
        bool    mNameWithPID;
        
        string  mLogFileFullpath;
        int mLogMaxSize;
	};

	//! 日志管理器(拥有多个日志对象)
	//！通过LogDriver创建日志对象, 在所有对象上逐一输出日志
	class LIBAPS_API LogManager
	{
	public:
		LogManager();
		~LogManager();

		Logger* getLogger(string name);
		void registerLogger(Logger*pNewLogger);

		void setDefaultLogger(Logger*pLogger);
        Logger * getDefaultLogger();
        void log(SourceInfo sourceInfo, LogLevel_t level, const char*szLog);
        void dumpBinary(SourceInfo si, LogLevel_t level, const unsigned char *in, int len);

        static LogManager& instance();
	private:
		Mutex mLock;
        map<string, Logger*> mLoggerMap;
        Logger *mDefaultLogger;
	};

	//! 仅类库内部使用.
    LIBAPS_API LogManager& getInternalLogManager();

    //! 日志代理(用完就扔)
    class LIBAPS_API LoggerAgent 
    {
    public:
        LoggerAgent(SourceInfo si, LogLevel_t level, Logger*pLoggerTo=NULL);
        LoggerAgent(SourceInfo si, LogLevel_t level, LogManager*pManager);

        ~LoggerAgent();
        
        inline LoggerAgent& operator<<( const formatStr& val )
        {
            oss << val.c_str();
            return *this;
        }

        template<class T>
        inline LoggerAgent& operator<<( T val )
        {
            oss << val;
            return *this;
        }

        void log(SourceInfo sourceInfo, LogLevel_t level, const char*fmt ...);

    private:
        void log_(SourceInfo sourceInfo, LogLevel_t level, const char*szLog);
 
        Logger*pLoggerTo_;
        LogLevel_t level_;
        SourceInfo si_;
        LogManager *pLogMan_;
        ostringstream oss;
        bool mFlushAtEnd;
    };

	#define Z_LOG(l) LoggerAgent(Z_SOURCEINFO, (l))
    #define Z_LOG_X(l) LoggerAgent(Z_SOURCEINFO, (l), &getInternalLogManager())
    #define Z_LOG_TO(l, logger) LoggerAgent(Z_SOURCEINFO, (l), (logger))

#ifndef _WIN32
    #define Trace(l, fmt, args ...) LoggerAgent(Z_SOURCEINFO, (l)).log(Z_SOURCEINFO, (l), (fmt), ##args)
    #define TraceX(l, fmt, args ...) LoggerAgent(Z_SOURCEINFO, (l), &getInternalLogManager()).log(Z_SOURCEINFO, (l), (fmt), ##args)
#else
    #define Trace(l, fmt, ...) LoggerAgent(Z_SOURCEINFO, (l)).log(Z_SOURCEINFO, (l), (fmt), __VA_ARGS__)
    #define TraceX(l, fmt, ...) LoggerAgent(Z_SOURCEINFO, (l), &getInternalLogManager()).log(Z_SOURCEINFO, (l), (fmt), __VA_ARGS__)
	
#endif 

    //! 用于跟踪代码运行及性能分析
    class LIBAPS_API ProfileTracer
    {
    public:
        ProfileTracer(const SourceInfo& si, LogLevel_t level_=eTRACE, const char*pName=NULL);
        ~ProfileTracer();

    private:
        //clock_t m_start;
        struct timeval m_start;
        SourceInfo m_si;
        LogLevel_t level;
        string m_name;
    };

    #define Z_TRACECODE aps::ProfileTracer _footprint(Z_SOURCEINFO);
    #define Z_TRACECODE_EX(l) aps::ProfileTracer _footprint(Z_SOURCEINFO, l);

    void LIBAPS_API fatalError(const char*fmt ...);
}

#endif // !__LIBAPS_LOGGER_HPP__