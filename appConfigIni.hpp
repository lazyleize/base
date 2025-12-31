/*
 * @file        appConfigIni.hpp
 * @brief       配置文件类
 *
 * @author      leize<leize@xgd.com>
 * @date        2017/09/01
 * @copyright   Copyright (c) 2021 xgd 
*/
#ifndef __LIBAPS_APPCONFIG_INI_HPP__
#define __LIBAPS_APPCONFIG_INI_HPP__

#include <base/appConfig.hpp>
#include <sstream>
#include <string>

namespace aps
{
	using namespace std;
	
	//! INI配置文件
	class LIBAPS_API AppConfigIni:public AppConfig
	{
	public:
		AppConfigIni();
		AppConfigIni(string path);
		virtual ~AppConfigIni();

		//! 重新加载配置
        virtual bool load();

        //! 保存修改
        virtual void save();

        //! 另存为[可以是文件/数据库]
        virtual bool saveAs(string path);

        //! 存储对象是否更新
        virtual bool isSourceModified();

	private:
		AppConfigIni(const AppConfigIni&);
        AppConfigIni& operator=(const AppConfigIni&);
	};
}

#endif // !__LIBAPS_APPCONFIG_INI_HPP__