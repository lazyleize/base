/*
 * @file        appConfigXML.hpp
 * @brief       配置文件(XML为载体)
 *
 * @author      leize<leize@xgd.com>
 * @date        2021/09/01
 * @copyright   Copyright (c) 2021 xgd 
*/
#ifndef __LIBAPS_APPCONFIGXML_HPP__
#define __LIBAPS_APPCONFIGXML_HPP__

#include <base/appConfig.hpp>
#include <sstream>
#include <string>

namespace aps
{
	using namespace std;

	//! XML配置文件
	class LIBAPS_API AppConfigXML:public AppConfig
	{
	public:

		AppConfigXML();
		AppConfigXML(string path);
		virtual ~AppConfigXML();

		//! 重新加载配置
        virtual bool load();

        //! 保存修改
        virtual void save();

        //! 另存为[可以是文件/数据库]
        virtual bool saveAs(string path);

        //! 存储对象是否更新
        virtual bool isSourceModified();

        //! 输出字符串
        virtual string toString();

    private:
        AppConfigXML(const AppConfigXML&);
        AppConfigXML& operator=(const AppConfigXML&);
	};
}

#endif // !__LIBAPS_APPCONFIG_XML_HPP__