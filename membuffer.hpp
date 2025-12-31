/*
* @file        memBuffer.hpp
* @brief       原始内存块简单封装
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#ifndef __LIBAPS_MEMBUFFER_HPP__
#define __LIBAPS_MEMBUFFER_HPP__

#include <base/config_.hpp>

#include <cstring>
#include <string>
#include <malloc.h>

namespace aps
{
	using namespace std;

	//! 轻量级内存操作类
	class LIBAPS_API MemBuffer
	{
	public:
		MemBuffer();
		MemBuffer(const MemBuffer&buf);
		MemBuffer& operator=(const MemBuffer& buf);
		~MemBuffer();

		//! 返回可用内存区(末尾置0)
		char* getBuffer();

		//! 返回可用内存区
		void* getRawBuffer()const;

		//! 已有数据大小
        const size_t& size() const ;

		//! 总共分配内存大小
		size_t totalMemory();

		//! 保证有len空闲
		int more(size_t len);

		//! 添加数据
		int append(const void *data,size_t datlen);

		//! 添加数据
		int append(MemBuffer& buf);

		//! 添加数据
		int append(const string& str);

		//! 至少总计size内存
		void reserve(size_t size_);

		//! 重置并初始化内存为'\0'
		void reset();

		//! 截断至长度len
		void truncate(size_t len);

		//! 打印到缓存
		int format(const char* fmt,...);

		//! 保存到文件
		int saveToFile(const string filePath,bool bAppend = false)const;

		//! 从文件读入
		int loadFromFile(const string filePath, size_t nOffset=0, size_t nCount=0);

		//! 调试信息
        void dumpInfo(size_t dumpCount=96);

	private:
		void _init(size_t preAlloc);

		// 有效内存移到头部
		void align();

		char* pBuffer_;
		char* pOrigBuffer_;

		size_t nMisalign_;
		size_t nTotalLength_;
		size_t nOff_;
	};
}
#endif //end __LIBAPS_MEMBUFFER_HPP__