/*
* @file        mutex.hpp
* @brief       互斥量,用于对可能被多个线程同时访问的资源加锁
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_MUTEX_HPP__
#define __LIBAPS_MUTEX_HPP__

#include <base/platform.hpp>
#include <base/exception.hpp>
#include <base/scopedLock.hpp>

namespace aps 
{
    //! 互斥量类(线程同步) 
    /*!  
    用于对可能被多个线程同时访问的资源加锁
    */
    class LIBAPS_API Mutex 
    {
    public:
        typedef aps::ScopedLock<Mutex> ScopedLock;

        Mutex(const char* szName = 0) noexcept(false);
        ~Mutex();

        //! 加锁
        void lock() noexcept(false);

        //! 解锁
        void unlock() noexcept(false);

    private:
        Mutex(const Mutex&);
        Mutex& operator=(const Mutex&);

        struct mutex_handle_t;
        mutex_handle_t* mHandle;
    };

}



#endif // !__LIBAPS_MUTEX_HPP__
