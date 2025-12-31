/*
 * @file        sharedLibrary.cpp
 * @brief       动载库管理类
 *
 *  代码移植自 Portable C++ Application Framework
 *
 * @author      Christian Prochnow <cproch@seculogix.de>
 * @copyright   xgd
*/
#include <base/sharedLibrary.hpp>
#include <base/toolkit.hpp>
#include <base/logger.hpp>
#include <base/strHelper.hpp>

#ifndef Z_OS_WIN32
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>

using namespace aps;

using namespace std;
struct SharedLibrary::dso_handle_t {};

SharedLibrary::SharedLibrary():
mHandle(0)
{
}

std::string SharedLibrary::getFilePath()
{
    return mFilePath;
}

void SharedLibrary::load(const string name, ldmode_t mode) noexcept(false)
{
    mFilePath = name;
    
    //mode = bindNow;
    int flags = 0;
//     switch(mode
//     {
//     case bindLazy:
//         flags = RTLD_LAZY;
//         break;
//     case bindNow:
        flags = RTLD_NOW;
//         break;
//     }

    errno = 0;
    mHandle = (dso_handle_t*)dlopen(name.c_str(), flags);
    Z_LOG_X(eTRACE) << formatStr("Load %s to #%p", name.c_str(), mHandle);
    if(!mHandle)
    {
        int r = errno;
        const char*p = dlerror();
        std::string errmsg = p==NULL?"":p;
        throw Exception(Z_SOURCEINFO, r, errmsg.c_str());
    }
}

SharedLibrary::~SharedLibrary()
{
    if (mHandle!=NULL)
    {
        Z_LOG_X(eTRACE) <<formatStr("Unload %p", mHandle);
        dlclose((void*)mHandle);
        mHandle = NULL;
    }
}

void* SharedLibrary::getAddr(string symbol)
{
   Z_ASSERT(mHandle!=NULL);
   return dlsym((void*)mHandle, symbol.c_str());
}

#endif
