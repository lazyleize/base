/*
 * @file        fileLock.cpp
 * @brief       文件锁
 * @author      leize<leize@xgd.com>
 *
 * @copyright   Copyright (c) 2021 xgd
*/
#include <base/fileLock.hpp>
#include <base/exception.hpp>
#include <errno.h>
#include <cstdio>

using namespace aps;
using namespace std;

#ifdef _WIN32

struct FileLock::private_t
{
    string filePath;
    HANDLE hd;
};

aps::FileLock::FileLock(const string& file,bool bTry):mOK(false),private_(new FileLock::private_t)
{
	private_->filePath = file + ".lock";
	private_->hd = CreateFileA(private_->filePath.c_str(),GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_ARCHIVE,NULL);

	if(private_->hd!=INVALID_HANDLE_VALUE)
	{
		mOK = SetLock(true,bTry);
		if(bTry && mOK)
			throw Exception(Z_SOURCEINFO,-1,"FileLock fail!");
	}
}

aps::FileLock::~FileLock()
{
	if(mOK)
		SetLock(false);

	if(private_->hd != INVALID_HANDLE_VALUE)
	{
		CloseHandle(private_->hd);
		DeleteFileA(private_->filePath.c_str());
	}

	delete private_;
	private_ = NULL;
}

bool aps::FileLock::SetLock(bool doLock,bool bTry)
{
	// lock/unlock first byte in the file
	OVERLAPPED ov;
	ov.Offset = ov.OffsetHigh = 0;
	BOOL ret;
	if(doLock)
	{
		// http://msdn.microsoft.com/en-us/library/ms891385.aspx
		if(!bTry)
			ret = LockFileEx(private_->hd, LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &ov);
		else
			ret = LockFileEx(private_->hd, LOCKFILE_FAIL_IMMEDIATELY|LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &ov);
	}
	else
		ret = UnlockFileEx(private_->hd,0,1,0,&ov);
	return ret != 0;
}

#else
struct FileLock::private_t
{
    string filePath;
    int fd;
};

FileLock::FileLock(const std::string& file, bool bTry)
:mOK(false),
private_(new FileLock::private_t)
{
    private_->filePath = file + ".lock";
    // File must be open with O_WRONLY to be able to set write-locks.
    private_->fd = ::open(private_->filePath.c_str(), O_CREAT | O_WRONLY, 0600);
    if (private_->fd != -1) 
    {
        mOK = SetLock(true, bTry);
        if (bTry && !mOK)
            throw Exception(Z_SOURCEINFO, -1, "FileLock fail!");
    }
}

FileLock::~FileLock() 
{
    if (mOK) 
        SetLock(false);

    if (private_->fd != -1)
    {
        close(private_->fd);
        unlink(private_->filePath.c_str());
    }

    delete private_;
    private_ = NULL;
}

///  成功返回true
bool FileLock::SetLock(bool doLock, bool bTry) 
{
    struct flock lock;
    lock.l_type = (doLock ? F_WRLCK : F_UNLCK);

    // Lock the entire file
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    while (true) 
    {
        if (bTry)
        {
            struct flock fl;
            fl.l_type = F_WRLCK;
            fl.l_whence = SEEK_SET;
            fl.l_start = 0;
            fl.l_len = 0;
            if (fcntl(private_->fd, F_SETLK, &fl) == -1) 
            {
                if (errno  == EAGAIN || errno == EACCES)
                    return false;
                else
                    throw Exception(Z_SOURCEINFO, -1, "Unknown error");       
            }
            return true;
        }
        else 
        {
            if (fcntl(private_->fd, F_SETLKW, &lock) == 0) 
            {
                return true;
            } 
            else if ((errno != EACCES) && (errno != EAGAIN) && (errno != EINTR))
            {
                break;
            }
        }
    }

    return false;
}

#endif 
