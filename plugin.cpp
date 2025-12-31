/*
* @file        plugin.hpp
* @brief       插件实现类
*
*  代码移植自 Portable C++ Application Framework
*
* @author      Christian Prochnow <cproch@seculogix.de>
* @copyright   xgd
*/

#include <base/plugin.hpp>
#include <base/sharedLibrary.hpp>
#include <base/directory.hpp>
#include <base/file.hpp>
#include <base/logger.hpp>
#include <cstdio>
#include <cstring>

using namespace aps;

typedef PluginBase* (*plugin_create_t)(void);
typedef void (*plugin_destroy_t)(PluginBase*);

PluginBase::PluginBase()
{}

PluginBase::~PluginBase()
{}

struct PluginSharedLib 
{
    SharedLibrary* shlib;
    int refCount;
};

struct PluginFactoryImpl::Plugin 
{
    PluginSharedLib* lib;
    int refCount;
    PluginMetaInfo* meta;
};

PluginFactoryImpl::PluginFactoryImpl(const char* iface):mIface(iface)
{
}

PluginFactoryImpl::~PluginFactoryImpl()
{
	// 删除所有对象
	map<PluginBase*,Plugin*>::const_iterator pi = mPinst.begin();
	while(pi != mPinst.end())
	{
		Plugin* p = pi->second;
		p->meta->destroy(pi->first);
		--p->refCount;
		++pi;
	}

	mPinst.clear();

	// 卸载所有插件
	list<Plugin*>::const_iterator i = mPlugins.begin();
	while(i != mPlugins.end())
	{
		Plugin* p = *i;

		if(!(--p->lib->refCount))
			delete p->lib->shlib;

		delete p->lib;
		delete p;
		++i;
	}

	mPlugins.clear();
}

bool PluginFactoryImpl::unLoadFile(const string& path)
{
	Z_LOG_X(eTRACE) << "unLoadFile(" << path << ")";
	bool bOK = true;

	// 在对象列表中查找是否有属于DLL
	map<PluginBase*,Plugin*>::const_iterator pi = mPinst.begin();
	while(pi != mPinst.end())
	{
		Plugin *p = pi->second;
		if(p->lib->shlib->getFilePath()==path)
		{
			Z_LOG_X(eWARNING) << "Found PluginBase object, can't unload dll";
            return false;
		}
	}

	// 对象都已经 destroy
    // 查找有无dll
	list<Plugin*>::iterator i = mPlugins.begin();
	while(i != mPlugins.end())
	{
		Plugin *p = *i;

		if(p->lib->shlib->getFilePath()==path)
		{
			Z_LOG_X(eTRACE) << "dll Found, refCount:" << p->lib->refCount;
            if (p->lib->refCount<=0)
            {
                delete p->lib->shlib;
                delete p->lib;
                delete p;
                mPlugins.erase(i);
                return true;
            }
            else
            {
                bOK = false;
                break;
                //throw Exception(Z_SOURCEINFO, -1, "File in use");
            }
		}
		++i;
	}
	return bOK;
}

void PluginFactoryImpl::loadFile(const string& path)noexcept(false)
{
	ScopedLock<Mutex> locker(mLock);

	SharedLibrary* shlib = new SharedLibrary();
	Z_LOG_X(eTRACE) << "loadFile(" << path << ")";

	bool bRet = false;
	string errmsg = "";
	int iCode = -1;

	try
	{
		shlib->load(path,SharedLibrary::bindNow);
		bRet = true;
	}
	catch(Exception& e)
	{
		iCode = e.code();
		errmsg = e.what();
	}
	catch(std::exception& e)
	{
		errmsg = string("SharedLibrary error: ") + e.what();
	}
	catch(...)
	{
		errmsg = "Unknown SharedLibrary error";
	}

	if(!bRet)
	{
		delete shlib;
		shlib = NULL;
		throw Exception(Z_SOURCEINFO,iCode,errmsg);
	}

	Z_ASSERT(shlib!=NULL);
	PluginMetaInfo* meta = (PluginMetaInfo*)(*shlib).getAddr("ZFPT_plugin");
	if(!meta)
	{
		Z_LOG_X(eTRACE) << path << " is not dynamic link library or NO symbol 'ZFPT_plugin' exported!";
		delete shlib;
		return;
	}

	PluginSharedLib* lib = new PluginSharedLib;
	lib->shlib    = shlib;
	lib->refCount = 0;

	int i = 0;

	while(meta[i].iface)
	{
		Z_LOG_X(eTRACE) << "found iface: " << meta[i].iface << " feature:" << meta[i].feature;

		if(mIface == meta[i].iface && !find(meta[i].feature))
		{
			Plugin* plugin	  = new Plugin;
			plugin->lib		  = lib;
			plugin->refCount  = 0;
			plugin->meta      = &meta[i];

			++lib->refCount;
			mPlugins.push_back(plugin);
		}
		++i;
	}
	if(!lib->refCount)
	{
		delete lib;
		delete shlib;
	}
}

void PluginFactoryImpl::loadDir(const string& path)noexcept(false)
{
	Z_LOG_X(eTRACE) << "loadDir(" << path << ")";
	Directory dir(path);
	FileInfo fileInfo;

	for(;dir.getNextFile(fileInfo);)
	{
		if(fileInfo.isFile())
		{
			string suffix = fileInfo.suffix();

			if(suffix=="so"||suffix=="dll"||suffix=="zmod")
			{
				Z_LOG_X(eTRACE) << "Loading plugin " << fileInfo.fullpath() << "";
                loadFile(fileInfo.fullpath().c_str());
			}
		}
	}
}

PluginFactoryImpl::Plugin* PluginFactoryImpl::find(const char* feature)const
{
	ScopedLock<Mutex> locker(mLock);

	list<Plugin*>::const_iterator i = mPlugins.begin();
	while(i != mPlugins.end())
	{
		Plugin *p = *i;
		if(string(p->meta->feature)==feature)
			return p;
		++i;
	}
	return 0;
}

PluginBase* PluginFactoryImpl::create(const string &feature)
{
	ScopedLock<Mutex>locker(mLock);

	Plugin* p = find(feature.c_str());
	if(p)
	{
		Z_LOG_X(eTRACE) << "Feature ["<< feature << "] found. iface=[" << p->meta->iface<<"]";
		PluginBase* addr = p->meta->create();
		Z_ASSERT(addr!=NULL);

		++p->refCount;

		mPinst[addr] = p;
		return addr;
	}
	else
		Z_LOG_X(eTRACE) << "No feature ["<< feature << "] found.";

	return 0;
}

void PluginFactoryImpl::destroy(PluginBase* pluginInst)
{
	ScopedLock<Mutex>locker(mLock);

	Plugin* p = mPinst[pluginInst];
	if(p)
	{
		p->meta->destroy(pluginInst);
		--p->refCount;

		mPinst.erase(pluginInst);
	}
}

bool PluginFactoryImpl::provides(const string& feature)const
{
	Plugin *p = find(feature.c_str());
	return (p)?(true):(false);
}

void PluginFactoryImpl::dump() const
{
    ScopedLock<Mutex> locker(mLock);
    
    list<Plugin*>::const_iterator i = mPlugins.begin();
    while(i != mPlugins.end())
    {
        Plugin* p = *i;
        Z_LOG_X(eTRACE) << "feature: ["<< p->meta->feature << "/" << p->meta->iface<< "]";
        
        i++;
    }
}
