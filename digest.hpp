/*
* @file        digest.hpp
* @brief       常用摘要算法
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_DIGEST_HPP__
#define __LIBAPS_DIGEST_HPP__

#include <base/platform.hpp>
#include <base/exception.hpp>
#include <string>

namespace aps
{
	using namespace std;

	//! 摘要算法基类
	class LIBAPS_API Digest
	{
	public:
		Digest();
		virtual ~Digest();

		//! 更新摘要: 一般摘要都是对大块内存分块计算的.
		virtual void update(const char* buff,size_t len) = 0;

		//! 处理字符串
		string fromString(string buffer);

		//! 从文件读取
		string fromFile(string filePath)noexcept(false);

		//! 获取结果
		virtual string digest() = 0;

		//! 清算结果
		virtual void clear()=0;
	};
}


#endif // !__LIBAPS_UTILITY_HPP__
