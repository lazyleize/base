/*
* @file        directory.win32.cpp
* @brief       目录操作简单封装
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#include <base/directory.hpp>
#include <base/strHelper.hpp>
#include <base/exception.hpp>
#include <base/file.hpp>

#ifdef Z_OS_WIN32

#include <stack>
#include <sys/stat.h>
#include <direct.h>
#include <logger.hpp>

using namespace aps;
using namespace std;

struct Directory::dir_handle_t 
{
	HANDLE handle;
	WIN32_FIND_DATA current;
	bool first, eof;
};

Directory::Directory(string path)
:mDirInfo(NULL), mHandle(NULL), mSeparator("\\")
{
	mHandle = new dir_handle_t;
	mHandle->handle = FindFirstFile((string(path) + "\\*").c_str(), &mHandle->current);
	if(mHandle->handle == INVALID_HANDLE_VALUE)
	{
		delete mHandle;
		mHandle = NULL;
		int r = Toolkit::getLastErrorNo();
		string err = StrHelper::trim(Toolkit::formatError(r)) + "(" +path + ")";
		throw Exception(Z_SOURCEINFO, r, err);
	}
	mHandle->first = true;
	mHandle->eof   = false;

	//Z_LOG_X(eTRACE) << "mHandle->current.cFileName=" << mHandle->current.cFileName;

	mDirInfo = new FileInfo(path);
}


Directory::~Directory()
{
	if (mHandle!=NULL)
	{
		FindClose(mHandle->handle);
		delete mHandle;
	}

	if (mDirInfo!=NULL)
		delete mDirInfo;
}

extern void FileTimeToDateTime(FILETIME* ft, Datetime& dt);


bool Directory::getCurFile(FileInfo &fileInfo) noexcept(false)
{
	fileInfo.reset();
	if(!mHandle->eof)
	{
		fileInfo.setName(mHandle->current.cFileName);
		fileInfo.setPath(mDirInfo->fullpath());
		fileInfo.setType(eFileTypeFile);

		if (mHandle->current.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			fileInfo.setType(eFileTypeDirectory);

		LARGE_INTEGER fsli;
		fsli.LowPart  = mHandle->current.nFileSizeLow;
		fsli.HighPart = mHandle->current.nFileSizeHigh;
		fileInfo.setSize(fsli.QuadPart);

		Datetime tm_;
		FileTimeToDateTime(&mHandle->current.ftCreationTime, tm_);
		fileInfo.setCtime(tm_);

		FileTimeToDateTime(&mHandle->current.ftLastWriteTime, tm_);
		fileInfo.setMtime(tm_);

		FileTimeToDateTime(&mHandle->current.ftLastAccessTime, tm_);
		fileInfo.setAtime(tm_);

		//Z_LOG_X(eTRACE) << "mHandle->current.cFileName=" << mHandle->current.cFileName;
		//Z_LOG_X(eTRACE) << "mHandle->current.cFileName=" << fileInfo.name();

		return true;
	}

	return false;
}


bool Directory::getNextFile(FileInfo &fileInfo) noexcept(false)
{
	fileInfo.reset();

	if(mHandle->first)
	{
		mHandle->first = false;
		return getCurFile(fileInfo);
	}

	if(!FindNextFile(mHandle->handle, &mHandle->current))
	{
		if(GetLastError() == ERROR_NO_MORE_FILES)
			mHandle->eof = true;
		else
			throw Exception(Z_SOURCEINFO, GetLastError(), "无法读取目录下的文件信息.");
	}

	return getCurFile(fileInfo);
}


void Directory::rewind()
{
	FindClose(mHandle->handle);
	mHandle->handle = FindFirstFile((mDirInfo->fullpath(eDirSepStyleWIN) + "\\*").c_str(), &mHandle->current);
	mHandle->first  = true;
	mHandle->eof    = false;
}
#endif
