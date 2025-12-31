/*
* @file        plugin.hpp
* @brief       插件实现类
*
*  代码移植自 Portable C++ Application Framework
*
* @author      Christian Prochnow <cproch@seculogix.de>
* @copyright   xgd
*/
#ifndef __LIBAPS_PLUGIN_HPP__
#define __LIBAPS_PLUGIN_HPP__

#include <base/config_.hpp>
#include <base/logger.hpp>
#include <base/mutex.hpp>
#include <list>
#include <map>
#ifndef _WIN32
#include <typeinfo>
#endif

namespace aps
{
	using namespace std;

	//! 插件类的基类
    /*!
    仅仅用作基类无业务功能
    */
	class LIBAPS_API PluginBase
	{
	public:
		PluginBase();
		virtual ~PluginBase();
	};

	//! 插件信息
	struct PluginMetaInfo 
    {
        const char* iface;
        const char* feature;
        PluginBase* (*create)();
        void (*destroy)(PluginBase* c);
    };

#define Z_PLUGINS_BEGIN \
    extern "C" { \
    LIBAPS_EXPORT PluginMetaInfo ZFPT_plugin[] = {

#define Z_PLUGIN(iface, feature, cl) \
    { typeid(iface).name(), feature, &cl::create, &cl::destroy },

#define Z_PLUGINS_END \
    { 0, 0, 0, 0 } \
    }; }


	//! 插件载入功能实现
	class LIBAPS_API PluginFactoryImpl
	{
	public:
		PluginFactoryImpl(const char* iface);
		virtual ~PluginFactoryImpl();

		//! 加载
        void loadFile(const string& filePath) noexcept(false);

        //! 卸载
        bool unLoadFile(const string& path);

        //! 载整个目录中的插件
        void loadDir(const string& path) noexcept(false);

        //! 创建插件中实现的类
        PluginBase* create(const string& feature);

        //! 销毁通过create的对象(切勿自行delete)
        void destroy(PluginBase* inst);

        //! 判断某个功能是否在此插件中实现
        bool provides(const string& feature) const;

        //! 打印所有功能
        void dump() const;

	private:
		struct Plugin;
        Plugin* find(const char* feature) const;

        mutable Mutex mLock;
        std::string        mIface;
        std::list<Plugin*> mPlugins;
        std::map<PluginBase*, Plugin*> mPinst;
	};

	/* 插件管理类
     *
     * @code
     *   CPluginFactory<Driver> *pDriverLoader = new CPluginFactory<Driver>();
     *   pDriverLoader->loadPluginFile("abc.so");
     *   Driver*pDriver = pDriverLoader->create("abc");
     *   pDriverLoader->destroy(pDriver);
     * @endcode
     */
	template<class Iface>
	class PluginFactory:public PluginFactoryImpl
	{
	public:
		inline PluginFactory():PluginFactoryImpl(typeid(Iface).name())
		{}

		inline ~PluginFactory()
		{}

		inline Iface* create(const char* feature)
		{
			Z_ASSERT(feature!=NULL);
			PluginBase *pObj = PluginFactoryImpl::create(feature);
			Iface *pObjDist = dynamic_cast<Iface*>(pObj);

			Z_LOG_X(eTRACE)<<"PluginBase = #"<<pObj<<", Iface*=#" << pObjDist;

			if(pObj!=NULL && pObjDist==NULL)
				Z_LOG_X(eTRACE) << "Convert object to type [" << typeid(Iface).name() << "] failed!";

			return pObjDist;
		}
		
		inline void destroy(Iface* inst)
		{
			Z_ASSERT(inst!=NULL);
			PluginFactoryImpl::destroy(dynamic_cast<PluginBase*>(inst));
		}
	};

}

//  内部实现原理
// ----------------------------------------------
// File : /path/to/X.plgin.so/dll
// ZFPT_plugin = 
// {
//    className1, "className1", create1, destroy1,
//     - Object1, Object2, Objec3 ...
//    className2, "className2", create2, destroy2,
//      - Object4, Object5, Objec6 ...
//    className3, "className3", create3, destroy3
//     - Object7, Object8, Objec9 ...
// }
// ----------------------------------------------
// 一个PluginFactory只能加载一个className
// 
// 平台只使用一种情况: 一个动态库只包含一个类.


// 以下宏纯粹方便使用.
// --------------------------------------------------------------

// 用于类声明
// --------------------------------------------------------------
#define Z_DECL_SO_API static PluginBase* create(); \
static void destroy(PluginBase* plugin);

// 用于实现文件.
// --------------------------------------------------------------
#define Z_IMPL_SO_API(handlerName, className, apiClasssName) \
    Z_PLUGINS_BEGIN \
    Z_PLUGIN(apiClasssName, handlerName, className) \
    Z_PLUGINS_END \
    \
    PluginBase* className::create()\
{ \
    PluginBase*pObj = NULL;\
    \
    try\
    {\
    pObj = new className();\
}\
    catch (aps::Exception& e)\
    {\
    fatalError("Exception: file:%s line:%d function:%s [%d] [%s]\n", e.file(), e.line(), e.func(), e.code(), e.what());\
}\
    catch (...)\
    {\
    fatalError("%s", "Unknown error.\n");\
}\
    return pObj;\
}\
    void className::destroy(PluginBase* plugin)\
{\
    delete plugin;\
}
// --------------------------------------------------------------
#endif // !__LIBAPS_PLUGIN_HPP__
