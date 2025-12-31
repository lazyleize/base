/*
 * @file    serverApplication.hpp
 * @brief   服务程序基类
 *
 * @author  leize<leize@xgd.com>
 *
*/
#ifndef __LIBAPS__SERVERAPPLICATION_HPP__
#define __LIBAPS__SERVERAPPLICATION_HPP__

#include <base/consoleApplication.hpp>
#include <base/mutex.hpp>
#include <base/json.hpp>
#include <string>
#include <map>

using namespace std;

namespace aps
{
	//! 服务程序信息类
	class LIBAPS_API ServerMetadata
	{
	public:
		ServerMetadata();
		virtual ~ServerMetadata();

		//! 取值
        string readAsString(string section, string key, string def="");

        //! 取值
        double readAsDouble(string section, string key, double def=0.0);

        //! 取值
        int readAsInt(string section, string key, int def=0);

        //! 取值
        bool readAsBool(string section, string key, bool def=false);

        //! 节点判断
        bool isSectionExists(string section);

        //! 值判断
        bool isKeyExists(string section, string key);

        //! 赋值
		void setKeyValue(const string& section, const string& key, string val);
        void setKeyValue(const string& section, const string& key, int val);
        void setKeyValue(const string& section, const string& key, double val);

        //! 删除
        void removeKey(const string& section, const string& key);
        void removeSection(const string& section);

		//! 输出字符串
        string toString();

	private:
		base::Value mIntalVal;
		Mutex mLock;
	};

	//! 服务程序基类(简化开发)
	class LIBAPS_API ServerApplication : public ConsoleApplication
    {
    public:
        ServerApplication(int argc, char**argv);
        virtual ~ServerApplication(){};

        //! 停止服务(可选)
        virtual void stop();
        
        //! 启动服务
        virtual void start();
    
        //! 是否将来停止
        bool isStopped();
        void setStopped(bool v);
        
        //! 是否已结束
        bool isFinished();
        void setFinished(bool v);

        //! 是否已启动
        bool isStarted();
        void setStarted(bool v);

        //! 服务名称
        string getServerName();

        //! 服务名称
        void setServerName(const string& name);
        
        ServerMetadata Inspect; //!< 每个服务程序都可向外暴露内部数据
    protected:
        
        string mServerName;
        bool mIsStarted;
        bool mIsFinished;
        bool mIsStopped;
    };
}

#endif //!__LIBAPS__SERVERAPPLICATION_HPP__