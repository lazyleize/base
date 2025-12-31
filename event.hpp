/*
* @file        event.hpp
* @brief       事件封装
* 摘自POCO
* @author      leize<leize@xgd.com>
* @copyright   xgd
*/
#ifndef __LIBAPS_EVENT_HPP__
#define __LIBAPS_EVENT_HPP__

#include <base/platform.hpp>
#include <base/exception.hpp>
#include <memory>

namespace aps 
{
    //! 事件类(线程同步)
    class LIBAPS_API Event
    {
    public:
        Event(bool autoReset = true);

        ~Event();

        void set();

        void wait();

        bool wait(long milliseconds);

        void reset();

    private:
        Event(const Event&);
        Event& operator = (const Event&);

        struct ev_handle_t;
        std::unique_ptr <ev_handle_t> mHandle;
    };
}

#endif // !__LIBAPS_EVENT_HPP__
