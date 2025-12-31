/*
* @file        exception.hpp
* @brief       简单的异常类
*
* @author      leize<leize@xgd.com>
* @copyright   xgd
*/
#ifndef __LIBAPS_EXCEPTION_HPP__
#define __LIBAPS_EXCEPTION_HPP__
#ifdef _WIN32
# pragma warning (disable:4251)
#endif 
#include <base/config_.hpp>
#include <base/sourceInfo.hpp>
#include <string>
namespace aps
{
	using namespace std;

	class LIBAPS_API Exception
	{
	public:
		Exception();
		Exception(SourceInfo sourceInfo,int iCode,const string& errMsg);
		Exception(const char* lpFile,int iLine,const char*lpFunc,int iCode, const string& errMsg);
		Exception(SourceInfo sourceInfo,const string& sCode,const string& errMsg);

		virtual ~Exception();

		// 拷贝函数
		Exception(const Exception& e);
		Exception& operator=(const Exception& e);

		//! 返回出错文件名
        const char* file() const;    

        //! 返回出错文件行数
        int line() const ;

        //! 返回出错函数
        const char* func() const;    

        //! 返回错误码
        int code() const;    

        //! 返回错误码
        const char* codes() const;

        //! 返回错误信息
        const char* what() const;    

        //! 默认异常处理函数
        static void defaultExceptionProcesser(Exception& ex);

		void setCode(int icode);
		void setWhat(const string& msg);
		void setSourceInfo(SourceInfo sourceInfo);

	protected:
		string mErrMsg;
		string mStrCode;
		int mIntErrCode;
		SourceInfo mSrcInfo;
	};

	//! 记日志的异常(外部使用)
    void LIBAPS_API logExceptionEx(SourceInfo sourceInfo, int iCode, const string& errMsg);
    void LIBAPS_API logException(SourceInfo sourceInfo, int iCode, const char* fmt, ...);

    void LIBAPS_API logExceptionEx(SourceInfo sourceInfo, const string& sCode, const string& errMsg);
    void LIBAPS_API logException(SourceInfo sourceInfo, const string& sCode, const char* fmt, ... );

    //! 记日志的异常(内部使用)
    void LIBAPS_API logExceptionX(SourceInfo sourceInfo, int iCode, const string& errMsg);
    void LIBAPS_API logExceptionX(const char* lpFile, int iLine, const char*lpFunc, int iCode, const string& errMsg);
}

#endif // !__LIBAPS_EXCEPTION_HPP__
