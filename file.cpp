/*
* @file        file.common.cpp
* @brief       文件通用操作
* @author      sunu<pansunyou@gmail.com>
*
* @copyright   Copyright (c) 2012 xgd
*/
#include <base/file.hpp>
#include <base/strHelper.hpp>
#include <base/exception.hpp>
#include <base/logger.hpp>
#include <errno.h>

using namespace aps;
using namespace std;

FileInfo::FileInfo(const string & fullpath)
{
	try
	{
		string path2 = fullpath;
		if(path_.find('/')!=string::npos&&path_.find('\\')!= string::npos)
			path2 = StrHelper::replace(path_,"\\","/");
		getFileInfo(path2);
	}
	catch(...){};
}

FileInfo::~FileInfo()
{
}

void FileInfo::reset()
{
	size_ = 0;
    type_ = eFileTypeUnknown;
    path_ = "";
    dirSepChar_ = "/";
    name_ = "";
}

//! 返回含文件名的完整路径
string FileInfo::fullpath(eDirSepStyle style)const
{
	string sep = dirSepChar_;
    if (style==eDirSepStyleWIN)
        sep = "\\";
    else if (style==eDirSepStyleUNIX)
        sep = "/";
	else
#ifdef Z_OS_WIN32
		sep = "\\";
#else
		sep = "/";
#endif
	    
    if (path_=="/"||path_=="\\"||path_=="")
        return path_ + name_; 
    else
		return path_ + sep + name_; 
}

//! 返回扩展名, 不含.号
string FileInfo::suffix()
{
    return getSuffix(name_);
}

//! 返回扩展名, 不含.号
string FileInfo::getSuffix(string path)
{
	string suffix = "";
	string::size_type pos = path.find_last_of(".");
	if(pos != string::npos)
		suffix = path.substr(pos+1);
	return suffix;
}

//! 纯名字[无扩展名]
const string FileInfo::pureName()
{
	string::size_type pos = name_.find_last_of(".");
	if(pos != string::npos)
		return name_.substr(0,pos);
	return name_;
}

//! 修改扩展名
string FileInfo::changeSuffix(string path,string newSuffix)
{
	string oldSuffix="";
	string::size_type pos = path.find_last_of(".");
	if(pos != string::npos)
		path = path.substr(0,pos+1) + newSuffix;
	else
		path = path + "." + newSuffix;
	return path;
}

//! 修改扩展名
FileInfo& FileInfo::changeSuffix(string newSuffix)
{
	name_=changeSuffix(name_,newSuffix);
	return *this;
}

string FileInfo::setDirSepStyle(string origPath,eDirSepStyle toStyle)
{
	if(toStyle == eDirSepStyleUNIX)
	{
		string::size_type pos = origPath.find_last_of("\\");
		if(pos!=string::npos)
			origPath = StrHelper::replace(origPath,"\\","/");

		pos = origPath.find_last_of("//");
		if(pos != string::npos)
			origPath = StrHelper::replace(origPath,"//","/");
	}
	else if(toStyle == eDirSepStyleWIN)
	{
		string::size_type pos = origPath.find_last_of("/");
		if(pos != string::npos)
			origPath = StrHelper::replace(origPath,"\\\\","\\");
	}
	return origPath;
}

//////////////////////////////////////////////////////////////////////////

File::File():Stream(),fp_(NULL)
{
	fp_ = NULL;
}

File::File(const string& path,const string& mode)noexcept(false):fp_(NULL)
{
	open(path,mode);
}

void File::open(const string& path,const string& mode)noexcept(false)
{
	path_ = path;
	openMode_ = mode;

	close();
	fp_ = fopen(path_.c_str(),mode.c_str());
	if(fp_ == NULL)
	{
		int r = 0;
#ifndef WIN32
		r = errno;
#else
		r = GetLastError();
#endif
		throw Exception(Z_SOURCEINFO,r,Toolkit::formatError(r));
	}
}

File::~File()
{
	try
	{
		close();
	}
	catch(...)
	{}
}

void File::reopen()noexcept(false)
{
	close();
	open(path_,openMode_);
}

FileInfo File::getFileInfo(const string& path)noexcept(false)
{
	FileInfo info;
	info.getFileInfo(path);
	return info;
}

bool File::isFile(const string& fullpath)
{
	bool ret = false;
	try
	{
		FileInfo v;
		v.getFileInfo(fullpath);
		ret = v.isFile();
	}
	catch(...)
	{}
	return ret;
}

bool File::isOpened()
{
	return fp_ != NULL;
}

void File::close()
{
	if(fp_!=NULL)
	{
		fclose(fp_);
		fp_=NULL;
	}
}

size_t File::write(const char* buffer,size_t n)
{
	if(!isOpened())
		throw Exception(Z_SOURCEINFO,-1,"file not opened");

	size_t nwritten = 0;
	size_t nleft = n;

	while(nleft > 0)
	{
		if((nwritten = fwrite(buffer,1,nleft,fp_)) <= 0)
		{
			if(EINTR == errno)
				continue;
			else
			{
				int r = Toolkit::getLastErrorNo();
				throw Exception(Z_SOURCEINFO,r,Toolkit::formatError(r));
			}
		}
		nleft -= nwritten;
		buffer += nwritten;
	}
	
	return n - nleft;
}

size_t File::read(char* buffer,size_t n)
{
	if(!isOpened())
		throw Exception(Z_SOURCEINFO,-1,"file not opened");

	size_t nread = 0;
	size_t nleft = n;

	while(nleft > 0)
	{
		if((nread = fread(buffer,1,nleft,fp_))<0)
		{
			if(EINTR == errno)
				continue;
			else
			{
				int r = Toolkit::getLastErrorNo();
				throw Exception(Z_SOURCEINFO,r,Toolkit::formatError(r));
			}
		}
		else if(nread == 0)
			break;

		nleft -= nread;
		buffer += nread;
	}
	return n - nleft;
}

bool File::seek(off_t offset,int mode)
{
	if(!isOpened())
		throw Exception(Z_SOURCEINFO,-1,"file not opened");

	return fseek(fp_,offset,mode) == 0;
}

const string & aps::File::path()const
{
	return path_;
}

void aps::File::unlink()noexcept(false)
{
	close();
	File::unlink(path_);
}

bool aps::File::move(const string& pathA,const string& pathB)
{
	return 0 == rename(pathA.c_str(),pathB.c_str());
}

off_t aps::FileInfo::size()const
{
	return (off_t)size_;
}

TInt64 aps::FileInfo::size64()const
{
	return size_;
}

void aps::FileInfo::setName( string _name )
{
    name_ = _name;
}

const string& aps::FileInfo::name() const
{
    return name_;
}

void aps::FileInfo::setPath( string _path )
{
    path_ = _path;
}

const string aps::FileInfo::path( eDirSepStyle toStyle/*=eDirSepStyleAuto*/ ) const
{
    return setDirSepStyle(path_, toStyle);
}

void aps::FileInfo::setSize( TInt64 s )
{
    size_ = s;
}

aps::eFileType aps::FileInfo::type() const
{
    return type_;
}

void aps::FileInfo::setType( eFileType t )
{
    type_ = t;
}

const Datetime& aps::FileInfo::ctime() const
{
    return ctime_;
}

void aps::FileInfo::setCtime( Datetime&time_ )
{
    ctime_ = time_;
}

const Datetime& aps::FileInfo::mtime() const
{
    return mtime_;
}

void aps::FileInfo::setMtime( Datetime&time_ )
{
    mtime_ = time_;
}

const Datetime& aps::FileInfo::atime() const
{
    return atime_;
}

void aps::FileInfo::setAtime( Datetime&time_ )
{
    atime_ = time_;
}

bool aps::FileInfo::isVirtual() const
{
    return (type_ & eFileTypeUnknown) == eFileTypeUnknown;
}

bool aps::FileInfo::isFile() const
{
    return (type_ & eFileTypeFile) == eFileTypeFile;
}

bool aps::FileInfo::isDirectory() const
{
    return (type_ & eFileTypeDirectory) == eFileTypeDirectory;
}

bool aps::FileInfo::isCharDevice() const
{
    return (type_ & eFileTypeCharDevice) == eFileTypeCharDevice;
}

bool aps::FileInfo::isBlockDevice() const
{
    return (type_ & eFileTypeBlockDevice) == eFileTypeBlockDevice;
}

bool aps::FileInfo::isLink() const
{
    return (type_ & eFileTypeLink) == eFileTypeLink;
}

bool aps::FileInfo::isPipe() const
{
    return (type_ & eFileTypePipe) == eFileTypePipe;
}
