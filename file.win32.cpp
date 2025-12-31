/*
* @file        file.win32.cpp
* @brief       文件操作
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#include <base/file.hpp>
#include <base/platform.hpp>
#include <base/toolkit.hpp>
#include <base/exception.hpp>
#include <base/charset.hpp>

#ifdef Z_OS_WIN32

#include <sys/stat.h>
#include <shlwapi.h>
#include <string>

using namespace aps;
using namespace std;

void FileTimeToDateTime(FILETIME* ft,Datetime& dt)
{
	TIME_ZONE_INFORMATION tzi;
	ULARGE_INTEGER uli;
	SYSTEMTIME stLocal;

	GetTimeZoneInformation(&tzi);
	uli.LowPart    = ft->dwLowDateTime;
	uli.HighPart   = ft->dwHighDateTime;
	uli.QuadPart   = uli.QuadPart - (tzi.Bias * 60 * 1000 * 100);

	FileTimeToSystemTime(ft,&stLocal);
	dt = Datetime(stLocal.wYear,stLocal.wMonth,stLocal.wDay,stLocal.wHour+8,stLocal.wMinute,stLocal.wSecond,stLocal.wMilliseconds);
}

FileInfo::FileInfo()noexcept(false)
{
	dirSepChar_ = "\\";
}

void FileInfo::getFileInfo(string fullpath_)noexcept(false)
{
	if(fullpath_.length()<=0)
		throw Exception(Z_SOURCEINFO,-1,("文件名为空！"));

	if(fullpath_.at(fullpath_.length()-1) == '\\' || fullpath_.at(fullpath_.length()-1) == '/')
        fullpath_.erase(fullpath_.length()-1,1);

	string::size_type namepos = fullpath_.find_last_of("\\");
	if(namepos == string::npos)
	{
		namepos = fullpath_.find_last_of("/");
		dirSepChar_ = "/";
	}
	else
		dirSepChar_ = "\\";

	if(namepos == string::npos)
	{
		path_ = "";
		name_ = fullpath_;
	}
	else
	{
		path_ = fullpath_.substr(0,namepos);
		name_ = fullpath_.substr(namepos + 1);
	}

	HANDLE hFile = CreateFile(fullpath_.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ|FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		int r = GetLastError();
		throw Exception(Z_SOURCEINFO,r,Toolkit::formatError(r));
	}

	BY_HANDLE_FILE_INFORMATION hFileInfo;
	if(!GetFileInformationByHandle(hFile,&hFileInfo))
	{
		CloseHandle(hFile);
		int r = GetLastError();
		throw Exception(Z_SOURCEINFO,r,Toolkit::formatError(r));
	}

	LARGE_INTEGER fsli;
    fsli.LowPart  = hFileInfo.nFileSizeLow;
    fsli.HighPart = hFileInfo.nFileSizeHigh;
    size_ = fsli.QuadPart;
    
    if(hFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        type_ = eFileTypeDirectory;
    }
    else
    {
        switch(GetFileType(hFile))
        {
        case FILE_TYPE_DISK:
            type_ = eFileTypeFile;
            break;

        case FILE_TYPE_CHAR:
            type_ = eFileTypeCharDevice;
            break;

        case FILE_TYPE_PIPE:
            type_ = eFileTypePipe;
            break;

        case FILE_TYPE_UNKNOWN:
        default:
            type_ = eFileTypeUnknown;
        }
    }

    CloseHandle(hFile);

    FileTimeToDateTime(&hFileInfo.ftCreationTime, ctime_);
    FileTimeToDateTime(&hFileInfo.ftLastWriteTime, mtime_);
    FileTimeToDateTime(&hFileInfo.ftLastAccessTime, atime_);

}

TInt64 File::size()
{
	if(!isOpened())
		throw Exception(Z_SOURCEINFO,-1,("文件未打开"));

	size_t pos = ftell(fp_);
	fseek(fp_,0,SEEK_END);

	size_t fileSize = ftell(fp_);
	fseek(fp_,pos,SEEK_SET);

	return fileSize;
}

void File::truncate(TInt64 offset)noexcept(false)
{
	if(!isOpened())
		throw Exception(Z_SOURCEINFO,GetLastError(),("文件未打开"));

	_fseeki64(fp_,offset,SEEK_SET);

	HANDLE hd = (HANDLE)_fileno(fp_);
	if(!SetEndOfFile(hd))
	{
		int r = GetLastError();
		throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
	}
}

void File::unlink(const string& path)noexcept(false)
{
	if(!File::exists(path.c_str()))
		return ;
	if(!DeleteFile(path.c_str()))
	{
		int r = GetLastError();
		throw Exception(Z_SOURCEINFO,r,Toolkit::formatError(r));
	}
}

bool File::exists(const string& path)noexcept(false)
{
	return PathFileExists(path.c_str())?true:false;
}

void File::mktemp(const string& prefix, File& file) noexcept(false)
{
    file.close();
    
    char tmpdir[MAX_PATH+1];
    char tmpname[MAX_PATH+1];

    GetTempPath(MAX_PATH, tmpdir);
    UINT ret = GetTempFileName(tmpdir, prefix.c_str(), 0, tmpname);
    if(ret == 0)
    {
        int r = GetLastError();
        throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
    }
    
    HANDLE handle = CreateFile(tmpname, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE, NULL);

    if(handle == INVALID_HANDLE_VALUE)
    {
        DWORD r = GetLastError();
        File::unlink(tmpname);
        throw Exception(Z_SOURCEINFO, r, Toolkit::formatError(r));
    }
    
    file.open(tmpname, "wb");
}

#endif
