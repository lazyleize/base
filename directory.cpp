/*
* @file        directory.common.cpp
* @brief       目录操作简单封装
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#include <base/directory.hpp>
#include <base/strHelper.hpp>
#include <base/exception.hpp>
#include <base/file.hpp>
#include <base/toolkit.hpp>

#include <stack>
#include <sys/stat.h>

#ifdef Z_OS_WIN32
#include <direct.h>
#else 
#include <errno.h>
#endif
//#include "logger.hpp"

using namespace aps;

using namespace std;

string Directory::separator(eDirSepStyle style/*=eDirSepStyleWIN*/)
{
    return mSeparator;
}


void Directory::setSeparator(char separator)
{
    if (separator!='\\' && separator!='/')
        separator = '/';
    mSeparator = separator;
}


bool Directory::getSubFiles(string dirPath, vector<FileInfo> &files, bool bIncludeDot)noexcept(false)
{
    if (dirPath=="")
        return false;

    try
    {
        Directory dir(dirPath);

        FileInfo file;
        for (;dir.getNextFile(file);)
        {
            //Z_LOG_X(eTRACE) <<bIncludeDot<< ", file.name()=" << file.name() << "";
        
            if (!bIncludeDot && (file.name()=="."||file.name()==".." || file.suffix()=="swp") )
                continue;

            files.push_back(file);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}


bool Directory::getAllSubFiles(string dirPath, vector<FileInfo>& files, bool bIncludeDot)
{
    stack<string> dirsContainer;
    dirsContainer.push(dirPath);

    try
    {
        // 检测当前目录是否存在或者可读
        Directory dir(dirPath);
    }
    catch(...)
    {
        return false;
    }

    for (;dirsContainer.size()>0;)
    {
        string path = dirsContainer.top();
        dirsContainer.pop();

        try
        {
            Directory dir(path);

            FileInfo file;
            for (;dir.getNextFile(file);)
            {
                //file.setPath(path);
				if (!bIncludeDot && (file.name()=="." || file.name()==".." || file.suffix()=="swp"))
                    continue;

                if (file.isDirectory() && (file.name()!="." && file.name()!="..") )
                {
                    //printf("file.fullpath()=%s, path=%s\n", file.fullpath().c_str(), path.c_str());
                    dirsContainer.push(file.fullpath());
                    files.push_back(file);
                }
                else
                {
                    files.push_back(file);
                }
            }
        }
        catch(...)
        {
        }
    }

    return true;
}


int Directory::rmdir(string fullpath, bool bFailWhenNoEmpty)
{
    fullpath = StrHelper::replace(fullpath, "\\", "/");
    fullpath = StrHelper::replaceLoop(fullpath, "//", "/");

    //Z_LOG_X(eTRACE) << "rmdir(fullpath=" << fullpath << ")";
    vector<FileInfo> files;
    Directory::getSubFiles(fullpath, files, false);
    //Z_LOG_X(eTRACE) << "rmdir(files=" << files.size() << ")";

    vector<FileInfo>::iterator it = files.begin();
    for (;it!=files.end();it++)
    {
        if (it->isDirectory())
        {
            rmdir(it->fullpath(), bFailWhenNoEmpty);
        }
        else 
        {
#ifndef WIN32
            chmod(it->fullpath().c_str(), 07777);
#endif
            int r = unlink(it->fullpath().c_str());
            int errno_;
#ifndef WIN32
            errno_ = errno;
#else
            errno_ = GetLastError();
#endif
            if (r!=0 || File::isFile(it->fullpath()))
            {
                throw Exception(Z_SOURCEINFO, errno_, 
                    formatStr("Delete file (%s) failed: %s", 
                    it->fullpath().c_str(),
                    Toolkit::formatError(errno_).c_str()));
            }
        }
    }
    
    int r = ::rmdir(fullpath.c_str());
    int errno_ = errno;
    if (r!=0 || Directory::isDirectory(fullpath))
    {
        throw Exception(Z_SOURCEINFO, errno_, formatStr("Delete file failed:%s", Toolkit::formatError(errno_).c_str()));
    }
    
    return r;
}


int Directory::mkdir(string fullpath, bool bRecursive)
{
    int err;
    fullpath = StrHelper::replace(fullpath, "\\", "/");
    fullpath = StrHelper::replaceLoop(fullpath, "//", "/");

    vector<string> dirs;
    dirs = StrHelper::split(fullpath, '/');

    vector<string> paths;
    paths.push_back(fullpath);

    for (size_t i=dirs.size()-1; i>0; i--)
    {
        string tmpPath;
        for (size_t j=0; j<i; j++)
        {
            tmpPath += dirs[j];
            if (j < (i-1) )
                tmpPath += '/';
        }

        if (tmpPath!="")
            paths.push_back(tmpPath);
    }

    for (int i=paths.size()-1; i>=0; i--)
    {
	err = 0;
        if (!isDirectory(paths[i]))
        {
#ifndef Z_OS_WIN32
            err = ::mkdir(paths[i].c_str(), 0766);
#else
            err = ::mkdir(paths[i].c_str());
#endif
        }

        if (!isDirectory(paths[i]))
            return err;
    }

    return err;
}


bool Directory::isDirectory(string fullpath)
{
    bool ret = false;
    try
    {
        FileInfo fileInfo;
        fileInfo.getFileInfo(fullpath);
        ret = fileInfo.isDirectory();
    }
    catch (...)
    {}

    return ret;
}


