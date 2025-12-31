/*
 * @file        sharedLibrary.win32.cpp
 * @brief       动载库管理类
 *
 *  代码移植自 Portable C++ Application Framework
 *
 * @author      Christian Prochnow <cproch@seculogix.de>
 * @copyright   xgd
*/
#include <base/sharedLibrary.hpp>

#ifdef Z_OS_WIN32
#include <base/charset.hpp>
#include <base/toolkit.hpp>
#include <base/logger.hpp>
#include <base/strHelper.hpp>

using namespace aps;
using namespace std;

struct SharedLibrary::dso_handle_t{};

SharedLibrary::SharedLibrary():mHandle(0)
{
}

string SharedLibrary::getFilePath()
{
	return mFilePath;
}

void SharedLibrary::load(const string name, ldmode_t mode) noexcept(false)
{
	mFilePath = name;

	mHandle = (dso_handle_t*)LoadLibrary(name.c_str());
	int r = GetLastError();
	Z_LOG_X(eTRACE) << formatStr("Loading %s to #%p", name.c_str(), mHandle);
	if(mHandle == 0)
		throw Exception(Z_SOURCEINFO,r,formatStr("Fail to load file [%s]: %s", name.c_str(), Toolkit::formatError(r).c_str()));
}

SharedLibrary::~SharedLibrary()
{
	if(mHandle!=0)
	{
		Z_LOG_X(eTRACE) <<  formatStr("Unload #%p", mHandle);
		FreeLibrary((HMODULE)mHandle);
	}
}

void* SharedLibrary::getAddr(string symbol)noexcept(false)
{
	Z_ASSERT(mHandle!=NULL);
	return GetProcAddress((HMODULE)mHandle,symbol.c_str());
}

#endif 
