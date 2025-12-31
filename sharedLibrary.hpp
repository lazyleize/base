/*
* @file        sharedLibrary.hpp
* @brief       动载库管理类
*
*  代码移植自 Portable C++ Application Framework
*
* @author      Christian Prochnow <cproch@seculogix.de>
* @copyright   xgd
*/
#ifndef __LIBAPS_SHAREDLIBRARY_HPP__
#define __LIBAPS_SHAREDLIBRARY_HPP__

#include <base/config_.hpp>
#include <base/exception.hpp>

namespace aps
{
	/// 动载库管理类

	/*
    用于动态加载dll/so并获取函数地址的类, 示例:
    @code
        CSharedLibrary shl("libm.so");
        double (*pCosine)(double) = NULL;
        pCosine = shl.getAddr("cos");
        pCosine(2.0);
    @endcode
    */
	class LIBAPS_API SharedLibrary
	{
	public:
		enum ldmode_t 
        {
            bindNow,  /*!< 马上加载 */
            bindLazy  /*!< 用到才加载 */
        };
		SharedLibrary();

		void load(string path, ldmode_t mode = bindLazy) noexcept(false);

		~SharedLibrary();

		//! 从动态库中获取函数地址
		void* getAddr(string symbol) noexcept(false);

		std::string getFilePath();

	private:
		SharedLibrary(const SharedLibrary&);
        SharedLibrary& operator=(const SharedLibrary&);

		struct dso_handle_t;
        dso_handle_t* mHandle;
        string mFilePath;
	};
}
#endif // !__LIBAPS_SHAREDLIBRARY_HPP__
