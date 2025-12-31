/*
* @file        scopedLock.hpp
* @brief       自动释放锁模板
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_SCOPEDLOCK_HPP__
#define __LIBAPS_SCOPEDLOCK_HPP__

namespace aps 
{
    //! 用于实现自动加锁解锁
    template <class M>
    class LIBAPS_API ScopedLock
    {
    public:
        ScopedLock(M& mutex): _mutex(mutex)
        {
            _mutex.lock();
        }

        ~ScopedLock()
        {
            _mutex.unlock();
        }

    private:
        M& _mutex;

        ScopedLock();
        ScopedLock(const ScopedLock&);
        ScopedLock& operator = (const ScopedLock&);
    };

    //! 用于实现自动加锁解锁
    template <class M>
    class LIBAPS_API ScopedUnlock
    {
    public:
        ScopedUnlock(M& mutex, bool unlockNow = true): _mutex(mutex)
        {
            if (unlockNow)
                _mutex.unlock();
        }

        ~ScopedUnlock()
        {
            _mutex.lock();
        }

    private:
        M& _mutex;

        ScopedUnlock();
        ScopedUnlock(const ScopedUnlock&);
        ScopedUnlock& operator = (const ScopedUnlock&);
    };

}

#endif // !__LIBAPS_SCOPEDLOCK_HPP__
