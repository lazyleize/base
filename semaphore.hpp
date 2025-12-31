/*
* @file        semaphore.hpp
* @brief       信号量
*  一般应用: 多进程使用同一资源池
*
* @author      leize<leize@xgd.com>
* @copyright   xgd
*/
#ifndef __LIBAPS_SEMAPHORE_HPP__
#define __LIBAPS_SEMAPHORE_HPP__

#include <base/platform.hpp>
#include <base/exception.hpp>

namespace aps
{
	//! 信号量
	class LIBAPS_API Semaphore
	{
	public:
		Semaphore(unsigned int initial = 0,const char* name = 0)noexcept(false);
		~Semaphore()noexcept(false);

		void wait()noexcept(false);
		bool tryWait()noexcept(false);
		void post()noexcept(false);

	private:
		struct sem_handle_t;
		sem_handle_t* mHandle;
	};
}

#endif // !__LIBAPS_SEMAPHORE_HPP__
