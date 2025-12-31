/*
* @file        directory.posix.cpp
* @brief       目录操作简单封装
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#include <base/directory.hpp>
#include <base/strHelper.hpp>
#include <base/exception.hpp>
#include <base/file.hpp>
#include <base/charset.hpp>

#if defined(Z_OS_LINUX) 

#include <stack>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

using namespace aps;
using namespace std;

struct Directory::dir_handle_t 
{
    DIR* handle;
    dirent* current;
    bool first;
    bool eof;

    dir_handle_t()
    {
        handle = NULL;
        current = NULL;
        first = false;
        eof = false;
    }
};

Directory::Directory(string path)
:mDirInfo(NULL), mHandle(NULL), mSeparator("\\")
{
    DIR* handle = opendir(path.c_str());
    int r = errno;
    if(!handle)
        throw Exception(Z_SOURCEINFO, r, "opendir() fail");

    mHandle = new dir_handle_t;
    mHandle->handle  = handle;

    mHandle->current = readdir(mHandle->handle);
    if(mHandle->current)
    {
        mHandle->first = true;
        mHandle->eof   = false;
    }
    else
    {
        mHandle->first = false;
        mHandle->eof   = true;
    }
    mDirInfo = new FileInfo(path);
}


Directory::~Directory()
{
    if (mHandle!=NULL)
    {
        closedir(mHandle->handle);
        delete mHandle;
    }

    if (mDirInfo!=NULL)
        delete mDirInfo;
}

bool Directory::getCurFile(FileInfo &fileInfo) noexcept(false)
{
    bool r = false;
    fileInfo.reset();
    if(!mHandle->eof && mHandle->current!=NULL)
    {
        string fullpath = mDirInfo->fullpath() + "/" + mHandle->current->d_name;
        
        try
        {
            fileInfo.getFileInfo(fullpath);
            r = true;
        }
        catch (Exception& e)
        { }
        catch (...)
        { }
    }
    return r;
}


bool Directory::getNextFile(FileInfo &fileInfo) noexcept(false)
{
    fileInfo.reset();

    if(mHandle->first)
    {
        mHandle->first = false;
        return getCurFile(fileInfo);
    }

    int r = errno;
    mHandle->current = readdir(mHandle->handle);
    if(!mHandle->current)
    {
        int r1 = errno;
        if (r1==r)
            mHandle->eof = true;
        else
            throw Exception(Z_SOURCEINFO, r1, "Can't read file infor");
    }
    
    return getCurFile(fileInfo);
}


void Directory::rewind()
{
    rewinddir(mHandle->handle);

    mHandle->current = readdir(mHandle->handle);
    if(mHandle->current)
    {
        mHandle->first = true;
        mHandle->eof   = false;
    }
    else
    {
        mHandle->first = false;
        mHandle->eof   = true;
    }
}

#endif
