/*
 * @file        appConfig.hpp
 * @brief       配置文件类
 *
 * @author      leize<leize@xgd.com>
 * @date        2017/09/01
 * @copyright   Copyright (c) 2021 xgd 
*/
#ifndef __LIBAPS_APPCONFIG_HPP__
#define __LIBAPS_APPCONFIG_HPP__

#include <base/platform.hpp>
#include <base/exception.hpp>
#include <base/datetime.hpp>
#include <sstream>
#include <string>
#include <map>
#include <cstdio>

namespace aps
{
	using namespace std;

	//! 配置接口文件类
	class LIBAPS_API AppConfig
	{
	public:
		typedef map<string,string>KeyValues_t;
		typedef map<string,map<string,string> >Sections_t;

		AppConfig();
		AppConfig(string path);
		virtual ~AppConfig();

		//! 重新加载配置
        virtual bool load() = 0;
        
        //! 保存修改
        virtual void save() = 0;

        //! 另存为[可以是文件/数据库]
        virtual bool saveAs(string path) = 0;

        //! 取值
        virtual string readString(string section, string key, string def="");

        //! 取值
        virtual double readDouble(string section, string key, double def=0.0);

        //! 取值
        virtual int readInt(string section, string key, int def=0);

        //! 取值
        virtual bool readBool(string section, string key, bool def=false);

        //! 节点判断
        virtual bool isSectionExists(string section);

        //! 值判断
        virtual bool isKeyExists(string section, string key);

        //! 存储对象是否更新
        virtual bool isSourceModified() = 0;

		//! 赋值
        virtual void setKeyValue(const string& section, const string& key, string val);
        virtual void setKeyValue(const string& section, const string& key, int val);
        virtual void setKeyValue(const string& section, const string& key, double val);
        
        //! 删除
        virtual void removeKey(const string& section, const string& key);
        virtual void removeSection(const string& section);
        
        //! 输出字符串
        virtual string toString();
        
        //! 路径
        string path();

        //! 设定路径 
        void setPath(string path_);

        //! 调试用
        void dump(FILE*fp=stderr);
        
        //! 调试用
        void dumpLog();

        //! 读取整个Section
        void getSection(const string&sectionName, KeyValues_t& section);

        //! 读取所有Section
        void getAllSection(Sections_t& sections);

        //! 清空
        void clear();

	protected:
		AppConfig(const AppConfig&);
        AppConfig& operator=(const AppConfig&);

        bool mModified;
        Datetime mLastLoadTime;
        string mConfigPath;
        Sections_t mSections;
	};
}

#endif // !__LIBAPS_APPCONFIG_HPP__