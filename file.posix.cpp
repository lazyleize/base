/*
* @file        file.cpp
* @brief       文件操作
* @author      sunu<pansunyou@gmail.com>
*
* @copyright   Copyright (c) 2012 xgd
*/
#include <file.hpp>


#if defined(Z_OS_LINUX) 

#include <base/strHelper.hpp>
#include <base/exception.hpp>
#include <base/toolkit.hpp>
#include <base/charset.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#if defined(Z_HAVE_LARGEFILE64) && !defined(Z_HAVE_OPEN64)
#define open64       open
#define ftruncate64  ftruncate
#define fstat64      fstat
#define stat64       stat
#endif

using namespace aps;

FileInfo::FileInfo()
{
    dirSepChar_ = "/";
}


void FileInfo::getFileInfo(string fullpath_) noexcept(false)
{
    if (fullpath_.length()<=0)
        return;

    if(fullpath_.at(fullpath_.length()-1) == '\\' || fullpath_.at(fullpath_.length()-1) == '/')
        fullpath_.erase(fullpath_.length()-1,1);

    string::size_type namepos = fullpath_.find_last_of("\\");
    if(namepos == string::npos)
    {
        namepos = fullpath_.find_last_of("/");
        dirSepChar_ = "/";
    }
    else
    {
        dirSepChar_ = "\\";
    }

    if(namepos == string::npos)
    {
        path_ = "";
        name_ = fullpath_;
    }
    else
    {
        path_ = fullpath_.substr(0,namepos);
        name_ = fullpath_.substr(namepos+1);
    }

    struct stat stbuf;

    if ( 0 != stat(fullpath_.c_str(), &stbuf) ) 
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, "stat() fail");
    }

    size_ = stbuf.st_size;

    struct tm *pTime= localtime(&stbuf.st_atime);
    atime_ = Date(pTime);

    pTime= localtime(&stbuf.st_ctime);
    ctime_ = Date(pTime);

    pTime= localtime(&stbuf.st_mtime);
    mtime_ = Date(pTime);

    type_ = eFileTypeUnknown;
    if(S_ISDIR(stbuf.st_mode))
    {
        type_ = eFileTypeDirectory;

        if (S_ISLNK(stbuf.st_mode))
            type_ = (eFileType) (type_ | eFileTypeLink);
    }
    else 
    {
/*
    S_ISLNK(st_mode)  is it a symbolic link
    S_ISREG(st_mode)  regular file
    S_ISDIR(st_mode)  directory
    S_ISCHR(st_mode)  character device
    S_ISBLK(st_mode)  block device
    S_ISFIFO(st_mode) fifo
    S_ISSOCK(st_mode) socket    
*/
        if (S_ISREG(stbuf.st_mode))
            type_ = eFileTypeFile;
        else if (S_ISCHR(stbuf.st_mode))
            type_ = eFileTypeCharDevice;
        else if (S_ISBLK(stbuf.st_mode))
            type_ = eFileTypeBlockDevice;
        else if (S_ISBLK(stbuf.st_mode))
            type_ = eFileTypeBlockDevice;

        if (S_ISLNK(stbuf.st_mode))
            type_ = (eFileType) (type_ | eFileTypeLink);
    }
}

//////////////////////////////////////////////////////////////////////////



void File::truncate(TInt64 offset) 
{
    int ret = ftruncate64(fileno(fp_), offset);
    int r = errno;
    if(ret == 0)
    {
        seek(offset, SEEK_SET);
        return;
    }

    throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
}

TInt64 File::size()
{
    struct stat64 stb;
    int ret = fstat64(fileno(fp_), &stb);
    int r = errno;
    if(ret == 0)
        return stb.st_size;

    throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
}

void File::unlink(const string& path) noexcept(false)
{
    if(::unlink(path.c_str()) == -1)
    {
        int r = errno;
        throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
    }
}

bool File::exists(const string& path)
{
    return ::access(path.c_str(), F_OK) ? false : true;
}

void File::mktemp(const string& prefix, File& file) noexcept(false)
{
    int len = prefix.length();
    char* tempnam = new char[len+10];
    memset(tempnam, 0x00, len+10);
    
    strcpy(tempnam, prefix.c_str());
    strcpy(tempnam + len, "XXXXXX");

    int handle = ::mkstemp(tempnam);
    int r = errno;
    string  path = tempnam;
    delete[] tempnam;

    if(handle == -1)
        throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
    
    file.open(path, "wb");
}

#endif
