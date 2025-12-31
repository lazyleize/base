/*
* @file        thread.hpp
* @brief       线程简单封装
*
* @author      leize<leize@xgd.com>
* @copyright   xgd
*/
#ifndef __LIBAPS_THREAD_HPP__
#define __LIBAPS_THREAD_HPP__

#include <base/config_.hpp>
#include <base/mutex.hpp>
#include <base/conditionVar.hpp>
#include <base/threadKey.hpp>

#include <string>
#include <vector>
#include <memory>
#include <list>
#include <stack>

namespace aps
{
	using namespace std;

	//! 线程任务基类
	class LIBAPS_API Runnable
	{
	public:
		Runnable(){};
		virtual ~Runnable(){};
		virtual void run() = 0;
	};

	class ThreadPool;
	typedef void (*Callable_t)(void*);

	//! 线程类
	class LIBAPS_API Thread
	{
	public:
		Thread();
		Thread(const string name);
		virtual ~Thread();

		//! 执行继承自Runnable的对象
		void start(Runnable* target, bool bSuspended=false);

		//! 执行线程函数
		void start(Callable_t target, void* pArgData = 0, bool bSuspended=false);

		//! 退出
        void stop();

		//! 执行暂停的线程
        void resume();

		//! 进程名
        string name();

		//! 等待线程结束
        //! 千万别在bSetDelAfter的时候调用
        void wait();

		//! 在运行吗
        bool isRunning();

		//! 线程结束后自动delete当前对象
        void deleteAfter(bool bSetDelAfter);

		//! 以毫秒以单位
        static void sleep(long milliseconds);

		//! 把CPU让出来
        static void yield();

		//! 当前进程对象, 主线(进)程调用时会返回0
        static Thread* getCurrentThread();

		//! 当前线程id(非pthread_t)
        static int getCurrentThreadId();

		static int createUniqueId();

	protected:
#ifndef _WIN32
		typedef unsigned int (*Entry)(void*);
#define THREAD_FUNC_RETURN void *
#else
		typedef unsigned int (__stdcall *Entry)(void*);
#define THREAD_FUNC_RETURN unsigned int __stdcall
#endif

		static THREAD_FUNC_RETURN runnableEntry(void* pThread);
        static THREAD_FUNC_RETURN callableEntry(void* pThread);
		void    create(Entry ent, void* pData);
        string  makeName();
        int     uniqueId();

	protected:
		Mutex mLock;             //!< 保护线程数据
        bool mIsSuspended;       //!< 创建线程后是否先暂停
        bool mIsDeleteThisAfter; //!< 线程函数退出时删除线程对象

		bool mIsRunning;         //!< 正在线程函数中
        bool mIsTerminated;      //!< 标志线程马上退出
        bool mIsDone;            //!< 线程函数执行完并退出

		int  mUniqueId;
        string mThreadName;
        int mThreadId;

		struct ThreadPrivate_t;
        std::unique_ptr<ThreadPrivate_t> mPrivate; //!< 内部使用

	private:
		static ThreadKey<Thread> mCurThreadHolder; //!< 存放当前线程对象地址, 见getCurrentThread

		// 不允许复制
        Thread(const Thread&);
        Thread& operator=(const Thread&);
        friend class ThreadPool;
	};

	/////////////////////////////////////////////
	class ThreadPool;
    class PooledThread;

	//! 线程池
	class LIBAPS_API ThreadPool
	{
	public:
		//! 创建线程池
        /// @param maxThreadCount 最大允许多少线程
        /// @param poolName       线程名
		ThreadPool(int maxThreadCount,string poolName="",int initThread = 0);
		~ThreadPool();

			//! 分配线程用于执行任务
        //! @param bWait 若为false且池满则直接返回
		bool dispatch(Runnable* target, bool bWait=true);

		//! 分配线程用于执行任务
        //! @param bWait 若为false且池满则直接返回
        bool dispatch(Callable_t target, void* pArgData=0, bool bWait=true);

		//! 停止服务
        void stop();

		//! 阻塞直到所有线程结束
        void wait();

		//! 当前多少闲置线程
        int idleCount();

        //! 当前有多少在执行任务
        int activeCount();

        //! 最大允许多少个线程
        int capacity();

        //! 更改线程池最大容量
        void setCapatity(int newSize);

        //! 新插入n个线程
        void ceateThreads(int n);

        //! 名字
        string name();

		//! 空函数啥事都不干
        static void nullThreadFunc(void*);

	private:
		Mutex mPoolMutex;                       //!< 线程池保护
        ConditionVar<Mutex> mHaveIdleCond;      //!< 有空闲线程
        ConditionVar<Mutex> mActiveEmptyCond;   //!< 无工作线程 
        ConditionVar<Mutex> mPoolEmptyCond;     //!< 池中无线程 

        int mMaxThreadCount;                    //!< 最大允许线程数
        string mPoolName;                       //!< 池名称
        bool mIsShutdown;                       //!< 关闭线程池
        std::stack<PooledThread *> mIdleTheads;     //!< 等待在线程函数上的空闲线程
        std::list<PooledThread *> mActiveTheads;    //!< 正在执行任务的繁忙线程
        
        bool saveThreadToIdel( PooledThread * pThread);
        void freeThread(PooledThread*pThread);
        friend class PooledThread;

        // 不允许复制
        ThreadPool(const ThreadPool& pool);
        ThreadPool& operator = (const ThreadPool& pool);
	};
}

#endif // !__LIBAPS_THREAD_HPP__
