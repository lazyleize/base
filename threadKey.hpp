/*
* @file        threadKey.hpp
* @brief       线程私有数据
* @author      Christian Prochnow <cproch@seculogix.de>
* @copyright   xgd
*/
#ifndef __LIBAPS_THREADKEY_HPP__
#define __LIBAPS_THREADKEY_HPP__

#include <base/platform.hpp>
#include <sstream>
#include <string>
#include <memory>

namespace aps 
{
    //! 线程私有数据操作类
    class LIBAPS_API ThreadKeyImpl 
    {
    public:
        ThreadKeyImpl();
        virtual ~ThreadKeyImpl();

        void set(void* ptr) throw();
        void* get() throw();

    private:
        ThreadKeyImpl(const ThreadKeyImpl&);
        ThreadKeyImpl& operator=(const ThreadKeyImpl&);

        struct key_handle_t;
        key_handle_t* m_handle;
    };

    //! 线程私有数据操作类
    /*
    使用示例(以char*为例):
    1. 全局的、函数内静态static变量或者类静态成员 ThreadKey<char> privateKey;
    2. 各线程(含主线程)即可调用privateKey.get()/set()来存取任意对象指针到线程私有的内存区域, 而互不影响.

    ThreadKey<X> 对象不可超过64个, 所以不要轻易将其做为类的私有变量.
    */
    template <class T>
    class LIBAPS_API ThreadKey: private ThreadKeyImpl 
    {
    public:
        inline ThreadKey()
            : ThreadKeyImpl() {}

        inline ThreadKey(const ThreadKey& k)
            : ThreadKeyImpl()
        { set(k.get()); }

        inline ~ThreadKey() {}

        inline ThreadKey& operator=(const ThreadKey& k)
        {
            ThreadKeyImpl::set(k.get());
            return *this;
        }

        ThreadKey& operator=(const T* val)
        {
            ThreadKeyImpl::set((void*)val);
            return *this;
        }

        void set(const T*p)
        { return ThreadKeyImpl::set((void*)p); }

        operator T* ()
        { return (T*)ThreadKeyImpl::get(); }

        T* get()
        { return (T*)ThreadKeyImpl::get(); }
    };

}

#endif // !__LIBAPS_THREADKEY_HPP__
