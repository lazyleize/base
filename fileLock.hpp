/*
 * @file        fileLock.hpp
 * @brief       文件锁
 *
 * @author      leize<leize@xgd.com>
 * @copyright   xgd
*/
#ifndef __LIBAPS_FILELOCK_HPP__
#define __LIBAPS_FILELOCK_HPP__

#include <base/config_.hpp>
#include <base/sourceInfo.hpp>

#include <fcntl.h>
#include <cerrno>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <string>

namespace aps
{
	/**
     * @brief 文件锁
     */
	class LIBAPS_API FileLock
	{
	public:
		FileLock(const std::string& file,bool bTry = false);
		~FileLock();

	private:
		FileLock(const FileLock&);
		FileLock& operator=(const FileLock&);

		bool SetLock(bool doLock,bool bTry = false);
		bool mOK;

		struct private_t;
		private_t* private_;
	};
}
#endif //! 
