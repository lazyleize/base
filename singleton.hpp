/*
* @file        singleton.cpp
* @brief       单例模板类
* @author      leize<leize@xgd.com>
*
* @copyright   
*/
#ifndef __LIBAPS_SINGLETON_HPP__
#define __LIBAPS_SINGLETON_HPP__

#include <base/mutex.hpp>
#include <base/initAps.hpp>
#include <typeinfo>

namespace aps
{
	//! 单件模式
    template<typename T>
    class Singleton 
    {
    public:
        static T& instanceEx()
        {
            if (pValue_==NULL)
                Singleton::init();
            return *pValue_;
        }
        
        static T* instance()
        {
            if (pValue_==NULL)
                Singleton::init();
            return pValue_;
        }

    protected:
        Singleton()
        {}

        virtual ~Singleton()
        {}

        //! 释放单例对象
        static void destroy()
        {
            //typedef char TMustBeCompleteType[sizeof(T) == 0 ? -1 : 1];
            delete pValue_;
        }
        
        //! 初始化对象
        static void init()
        {
            pValue_ = new T();
            ApsIniter::registerDestroyHandler(ApsIniter::registerHandle_t(destroy, typeid(T).name()));
        }

    private:
        Singleton(const Singleton&); 
        Singleton& operator=(const Singleton&); 
        
        static T* pValue_;
    };

    template<typename T>
    T* Singleton<T>::pValue_ = NULL;
}


#endif
