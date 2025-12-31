/*
 * @file        initAps.hpp
 * @brief       libaps初始化
 * @author      leize<leize@xgd.com>
 *
 * @copyright   xgd
*/
#ifndef __LIBAPS_INITAPS_HPP__
#define __LIBAPS_INITAPS_HPP__

#include <base/config_.hpp>
#include <base/mutex.hpp>

#include <vector>
#include <string>

namespace aps 
{
    //! 用于释放资源的函数指针原型
    typedef void (*destroyHandler_t)();

    //! 管理类库中资源的申请及释放
    class LIBAPS_API ApsIniter
    {
    public:
        /// @brief 注册注销函数
        struct LIBAPS_API registerHandle_t
        {
            destroyHandler_t pCallback;
            string name;
            registerHandle_t(destroyHandler_t pcb, string name_);
        };
    public:
        ApsIniter();
        ~ApsIniter();

        //! 登记需要类库托管的资源
        static void registerDestroyHandler(registerHandle_t v);

    protected:
        static Mutex mutex_;
        static int refCount_;
        static std::vector<registerHandle_t> destryHandlerList;
    };
}

#endif // !__LIBAPS_INITAPS_HPP__
