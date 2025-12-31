/*
 * @file        conditionVar.cpp
 * @brief       条件变量
 *
 *  代码移植自 POCO
 *
 * @author      POCO
 * @copyright   
*/
#ifndef __LIBAPS_CONDITIONVAR_HPP__
#define __LIBAPS_CONDITIONVAR_HPP__

#include <base/platform.hpp>
#include <base/exception.hpp>
#include <base/mutex.hpp>
#include <base/event.hpp>
#include <deque>

namespace aps 
{
    //! 条件变量类(线程同步)
    template <typename Mtx>
    class LIBAPS_API ConditionVar
    {
    public:
        ConditionVar(Mtx& friendMutex):
          mFriendMutex(friendMutex)
          {
          }

          ~ConditionVar(){};

          void notify()
          {
              Mutex::ScopedLock lock(mWaitQueueProtecter);
              if (!_waitQueue.empty())
              {
                  _waitQueue.front()->set();
                  dequeue();
              }
          }

          void wait()
          {
              ScopedUnlock<Mtx> unlock(mFriendMutex, false);
              Event event;
              {
                  Mutex::ScopedLock lock(mWaitQueueProtecter);
                  mFriendMutex.unlock();
                  enqueue(event);
              }
              event.wait();
          }

          bool tryWait(long milliseconds)
          {
              ScopedUnlock<Mtx> unlock(mFriendMutex, false);
              Event event;
              {
                  Mutex::ScopedLock lock(mWaitQueueProtecter);
                  mFriendMutex.unlock();
                  enqueue(event);
              }

              if (!event.wait(milliseconds))
              {
                  Mutex::ScopedLock lock(mWaitQueueProtecter);
                  dequeue(event);
                  return false;
              }
              return true;
          }

          void notifyAll()
          {
              Mutex::ScopedLock lock(mWaitQueueProtecter);
              for (auto it = _waitQueue.begin(); it != _waitQueue.end(); ++it)
              {
                  (*it)->set();
              }
              _waitQueue.clear();
          }

          Mtx& getMutex()
          {
              return mFriendMutex;
          }

    protected:

        void enqueue(Event& event)
        {
            _waitQueue.push_back(&event);
        }

        void dequeue()
        {
            _waitQueue.pop_front();
        }

        void dequeue(Event& event)
        {
            for (auto it = _waitQueue.begin(); it != _waitQueue.end(); ++it)
            {
                if (*it == &event)
                {
                    _waitQueue.erase(it);
                    break;
                }
            }
        }

    private:
        ConditionVar(const ConditionVar&);
        ConditionVar& operator = (const ConditionVar&);

        Mtx& mFriendMutex; 

        typedef std::deque<Event*> WaitQueue;
        Mutex mWaitQueueProtecter;
        WaitQueue _waitQueue;
    };

}

#endif // !__LIBAPS_CONDITIONVAR_HPP__
