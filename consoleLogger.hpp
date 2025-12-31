/*
* @file    consoleLogger.hpp
* @brief   日志打印到终端
* @author  leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_CONSOLELOGGER_HPP__
#define __LIBAPS_CONSOLELOGGER_HPP__

#include <base/platform.hpp>
#include <base/logger.hpp>
#include <base/consoleColor.hpp>
#include <string>

namespace aps 
{
    using namespace std;

    //! 终端日志类
    class LIBAPS_API ConsoleLogger : public Logger
    {
    public:
		ConsoleLogger();
        ConsoleLogger(string name, LogLevel_t level);
        virtual ~ConsoleLogger();

        virtual void start(const std::string& path);
        virtual void stop();
        virtual void restart();
        virtual bool isActive();
        virtual void log(SourceInfo sourceInfo, LogLevel_t level, const char*);

    private:
        FILE *mFileFd;
    };

#ifdef Z_OS_WIN32
	enum ColorPlan
	{
		WhiteOnBlack     = 0,
		GrayOnBlack      = 1,
		GreenOnBlack     = 2,
		AquaOnBlack      = 3,
		RedOnBlack       = 4,
		PurpleOnBlack    = 5,
		YellowOnBlack    = 6,
		LightBlueOnBlack = 7,
		LightGreenOnBlack,
		LightAquaOnBlack,
		LightRedOnBlack,
		LightPurpleOnBlack,
		LightYellowOnBlack,
		BrightWhiteOnBlack,
	};
	//! 终端输出绑定颜色
	class LIBAPS_API LoggerBindColor:public ConsoleLogger
	{
	public:
		LoggerBindColor(string name="",LogLevel_t level=eTRACE);
		//LoggerBindColor();
		virtual ~LoggerBindColor();

		void setColorScheme(ColorPlan cp,eConsoleColor bo=eConColorBlack);
		void severtColor();   //返回上一次的颜色方案
		void defColor();      //默认颜色
		void printf_colo(SourceInfo sourceInfo, LogLevel_t level, const char*);

	protected:
		 eConsoleColor oldf, oldb;
	
	};
	#define COLOR_LOG(l, fmt, ...) LoggerBindColor("stdout",(l)).printf_colo(Z_SOURCEINFO,(l),(fmt), __VA_ARGS__)
#endif

}

#endif // !__LIBAPS_CONSOLELOGGER_HPP__
