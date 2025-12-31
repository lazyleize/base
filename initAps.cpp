/*
 * @file        initAps.cpp
 * @brief       libaps初始化
 * @author      雷泽<leize@xgd.com>
 *
 * @copyright   xgd
*/
#include <base/initAps.hpp>
#include <base/logger.hpp>
#include <base/thread.hpp>

namespace aps 
{    
    ApsIniter::ApsIniter()
    {
        ScopedLock<Mutex> lock(mutex_);
        refCount_++;

        //Z_LOG_X(eTRACE) << "Initialize aps library <<< ["<<refCount_ <<"]";
    }

    ApsIniter::~ApsIniter()
    {
        ScopedLock<Mutex> lock(mutex_);
        refCount_--;
        //Z_LOG_X(eTRACE) << "Uninitialize aps library >>> ["<<refCount_+1 <<"]";
        if (refCount_<=0)
        {
            //Z_LOG_X(eTRACE) << "Call free functions ... ";
            for (size_t i=0;i<destryHandlerList.size();i++)
            {
                registerHandle_t& funcFree = destryHandlerList[i];
                //Z_LOG_X(eTRACE) << "Freeing object("<<(i+1) << ") ["<< funcFree.name<< "] with callback #"<<funcFree.pCallback<< " ... ";
                if (funcFree.pCallback!=NULL)
                    funcFree.pCallback();
                //Z_LOG_X(eTRACE) <<"Object("<<(i+1)<< ") ["<< funcFree.name<< "] freed";
            }
            destryHandlerList.clear();
            //Z_LOG_X(eTRACE) << "All singleton objects released";
       }
    }

    void ApsIniter::registerDestroyHandler(ApsIniter::registerHandle_t handler)
    {
        Z_LOG_X(eTRACE) << "register singleton object(["<<handler.name<<"], #"<<handler.pCallback <<")";
        Z_ASSERT(handler.pCallback!=NULL);
        ScopedLock<Mutex> lock(mutex_);
        destryHandlerList.push_back(handler);
    }

    Mutex ApsIniter::mutex_;
    int ApsIniter::refCount_ = 0;
    std::vector<ApsIniter::registerHandle_t> ApsIniter::destryHandlerList;

    ApsIniter::registerHandle_t::registerHandle_t( destroyHandler_t pcb, string name_ )
    {
        pCallback = pcb;
        name = name_;
    }
}

