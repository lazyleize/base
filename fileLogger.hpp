/*
* @file    fileLogger.hpp
* @brief   日志打印到文件
* @author  leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_FILELOGGER_HPP__
#define __LIBAPS_FILELOGGER_HPP__

#include <base/platform.hpp>
#include <base/logger.hpp>
#include <base/membuffer.hpp>

#include <string>

namespace aps
{
	using namespace std;
	class File;

	//! 文件日志类
	class LIBAPS_API FileLogger : public Logger
	{
	public:
		FileLogger(string name,LogLevel_t level);
		virtual ~FileLogger();

		virtual void start(const std::string& logFileDir);
		virtual void stop();
		virtual void restart();
		virtual bool isActive();
		virtual void log(SourceInfo sourceInfo, LogLevel_t level, const char*);
	private:

	};
}
#endif // !__LIBAPS_FILELOGGER_HPP__