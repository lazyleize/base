/*
* @file        toolkit.hpp
* @brief       常用工具类
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_UTILITY_HPP__
#define __LIBAPS_UTILITY_HPP__

#include <base/config_.hpp>
#include <base/mutex.hpp>
#include <sstream>
#include <string>
#include <cstdlib>
#include <vector>

namespace aps
{
	using namespace std;

	//! 无处安放的工具函数类
	class LIBAPS_API Toolkit
	{
	public:
		//! 替代系统的assert
		static void assertTrue(int line, const char*file, const char*strVal, bool val);

		//! 格式文件大小为 XXX KB 格式.
		static string formatFileSize(double fileSize, size_t suffixLen=2);

		 /**
        * @brief 将二进制数据每字节转换为十六字节字符
        *
        * @param inDat    二进制数据
        * @param length   长度
        * @retVal 
        */
		static string bin2hex(const void*inDat, size_t length, bool bUpper=false);

        static void bin2hex(const void*inDat, size_t inLength, unsigned char*outBuf, size_t outBufSize, bool bUpper=false);

		/**
        * @brief bin2hex 相反
        *
        * @param inHexStr     hex字符串
        * @param outBuffer    缓存区
        * @param bufferLen    缓存区长度
        * @param bZeroSuffix  长度
        * @retVal 
        */
        static bool hex2bin(string inHexStr, void*outBuffer, size_t bufferLen, bool bZeroTail=false);

        //! 格式化系统错误码
        static string formatError(int r=0xFFFFFFFF);

        //! 获取错误码
        static int getLastErrorNo();

        //! 当前进程ID
        static int getCurrentProcessId();

        //! 取伪随机数
        static double random(double start, double end);
    };

    #define Z_ASSERT(v) Toolkit::assertTrue(__LINE__, __FILE__, #v, (v));
}




#endif // !__LIBAPS_UTILITY_HPP__