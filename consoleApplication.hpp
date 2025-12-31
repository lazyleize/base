/*
* @file    consoleApplication.hpp
* @brief   终端程序基类
* @author  leize<leize@xgd.com>
* @copyright Copyright (c) 2021 xgd
*/
#ifndef __LIBAPS__CONSOLEAPPLICATION_HPP__
#define __LIBAPS__CONSOLEAPPLICATION_HPP__

#include <base/appConfigIni.hpp>
#include <base/mutex.hpp>
#include <base/logger.hpp>
#include <string>
#include <map>

namespace aps
{
	using namespace std;

	//! 参数处理类
	class LIBAPS_API ApplicationArgs
	{
	public:
		ApplicationArgs();
		virtual ~ApplicationArgs();

		//! 解析参数
        void parseArgs(int argc, char**argv);
                
        //! 取值
        int readArgInt(string arg, int defVal=0) const ;
        
        //! 取值
        TInt64 readArgInt64(string arg, TInt64 defVal=0)const ;
        
        //! 取值
        bool readArgBool(string arg, bool defVal=false)const ;
        
        //! 取值
        double readArgDouble(string arg, double defVal=0)const ;
        
        //! 取值
        string readArgString(string arg, string defVal="")const ;
        
        //! 取值
        string getopt(string arg, string defVal="")const ;
        
        //! 判断是否有某参数
        bool checkopt(string arg)const ;

        //! 复制所有参数
        void copyArgs(multimap<string, string>&toList)const ;
        
        //! 调试用
        void dump()const ;

	public:
		// 成员即数据
        int mArgc;
        char** mArgv;
        multimap<string, string> mArgMap;

        static ApplicationArgs &globalVal();

	protected:
		mutable Mutex mMutex;
	};

	//! 终端程序类(方便处理参数)
	class LIBAPS_API ConsoleApplication:public ApplicationArgs
	{
	public:
		ConsoleApplication(int argc,char**argv);

		virtual ~ConsoleApplication(){};

		virtual int run();

		//! 获取进程名
        string getAppFullname();

        //! 不带扩展名的进程名
        string getAppName();

        //! 获取程序所在目录
        string getAppPath();

        //! 初始化日志
        void setupLogger(string LogName, int Debug, LogLevel_t LogLevel, string FilePath, int maxSize);
        
        // 配置文件
        AppConfigIni mConfig;
	};
}

#endif //!__LIBAPS__CONSOLEAPPLICATION_HPP__