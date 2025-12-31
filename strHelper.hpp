/*
* @file        strHelper.hpp
* @brief       字符串工具
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_STRHELPER_HPP__
#define __LIBAPS_STRHELPER_HPP__

#include <base/platform.hpp>
#include <base/charset.hpp>
#include <sstream>
#include <string>
#include <map>
#include <vector>

namespace aps
{
	using namespace std;

	enum StrpadDirect_t
	{
		eStrpadDirectLeft = 0,
		eStrpadDirectRight = 1
	};
	//! 字符串工具集
	class LIBAPS_API StrHelper
	{
	public:
		/**
        * @brief 适用于GBK编码字符串的子串截取函数
        *
        * @param bigStr 母字符串
        * @param from   开始位置, 从0开始
        * @param length 长度
        * @retVal 子串
        */
		static string gbkSubstr(string bigStr, size_t from, size_t length);

		/**
        * @brief 适用于GBK编码字符串的子串截取函数
        *
        * @param origGbkStr  原字符串
        * @param bDelBadChar 是否删除非法字符
        * @param replaceBadChar 如果不删除字符,替换之以replaceBadChar
        * @retVal GBK编码字符串
        */
		static string getValidGbkString(string origGbkStr, bool bDelBadChar=false, char replaceBadChar=' ');

		//! 取随机字符串
		static string randomString(int length);

		//! 递归替换,直到不出现需要查找的字符为止.
		static string replaceLoop(string str, const string old_value, const string new_value);

		//! 顺序替换
		static string replace(string str,const string old_value,const string new_value); 
		static wstring replaceW(wstring str, const wstring old_value, const wstring new_value); 

		//! 第一个字符大写
        static string upperFirstChar(string str); 

		//! 第一个字符小写
        static string lowerFirstChar(string str);

		//! 大写
        static string toUpper(string str);

        //! 小写
        static string toLower(string str);

        //! 去空格
        static string trim(string inStr, const char*char2trim = "\r\n \t");

        //! 去空格
        static string trimLeft(string inStr, const char*char2trim = "\r\n \t");

        //! 去空格
        static string trimRight(string inStr, const char*char2trim = "\r\n \t");

        //! 字符串分割
        static vector<string> &split(const string &s, char delim, vector<string> &elems);

        //! 字符串分割
        static vector<string> split(const string &s, char delim); 
        
        //! 字符串分割
        static vector<string> split(const string& s, const string& delim, const bool keep_empty = true);

        //! 字符串拼整<默认右边>
        static string strpad(string instr, char charToFill, size_t lastStrLength, StrpadDirect_t dir=eStrpadDirectRight);

        //! 备用
        static string setKeyToExtString(string &bigString, string key, string val);

        //! 备用
        static string readKeyFromExtString(string bigstring, string key);

        //! 重复
        static string repeat(const string& instr, size_t count);

		/**
		*@brief 把数字替换成指定参数
		*@param iIndex 起始位置,从0开始,如果是反向替换,也是从0开始
		*@param iCnt 替换字符个数
		*@param c  用于替换的字符  
		*return 返回替换后字符串,如果失败,则返回空串
		*/
		static string SubNumWithChar(const string& str,const int& iIndex,const int iCnt,const char& c) ;

		//! 对手机号进行掩码
		static string MobileMask(const string& mobile,int mode=0);

		//! 对身份证号进行掩码
		/* mode:掩码模式
		0:进行显示前6位和后2位掩码
		如431121**********34[默认采用这种格式]
		1:进行后八位隐藏显示
		如4311211990********
		*/
		static string CredIdMask(const string& credid,int mode=0);

		//! 屏蔽银行卡号
		static void ShieldCard(string &sCreId, char cSymbol='*');
	};

	LIBAPS_API string double2string(double val, string format="%0.2lf");
    LIBAPS_API string int2string(int val, string format="%d");
	LIBAPS_API string bool2string(bool val, string format="false");

	 //! 字符串格式化
    // 用于字符串格式化,类似于sprintf,但更方便使用
	class LIBAPS_API formatStr
	{
	public:
		formatStr(const char* fmt ...);
		~formatStr();

		string str();

		const char* c_str()const;

		operator const char*();

		operator char*();

		operator string();

		formatStr& operator=(const formatStr& other);
        formatStr(const formatStr& other);

    private:
        formatStr();
        char *mBuffer;
	};

	//! KeyValue格式字符串
	class LIBAPS_API KVString 
	{
	public:
		KVString();
		KVString(const string& bigString);
		~KVString();

		string  getString(const string&key, string defVal="");
        int     getInt(const string&key, int defVal=0);
		bool	getBool(const string&key,bool defVal=false);
        double  getDouble(const string&key, double defVal=0);

		KVString& setString(const string&key, const string& val);
        KVString& setInt(const string&key, int val);
		KVString& setBool(const string&key, bool val);
        KVString& setDouble(const string&key, double val);
        KVString& removeKey(const string&key);

		string toString();
        void dump();
        KVString& parse(const string& bigString);

    private:
        string  getStr(const string&key, string defVal);
        void    processVal(string&key);
        void    unProcessVal(string&key);
        string  mBigString;

	public:
		KVString& operator=(const KVString&o);
		KVString(const KVString&);
	};

	LIBAPS_API int strlenUtf8(const char *s);

	typedef std::map<std::string,std::string> CStr2Map ;
	typedef vector<std::string> CStrVector ;

	//! CStr2Map工具
	class LIBAPS_API CStr2MapTool
	{
	public:
		//! 把字符串按标志符分开后存放到vector
		static int String2Vector(const string & sSrc,const string & sSep,std::vector<std::string> & vStr);

		//! 拷贝CStr2Map
		static void MapCpy(const CStr2Map& src,CStr2Map& dest);

		//! 把字符串按标志符分开后按名值对存放到map,Str= a=1&b=2 执行后,outMap["a"]="1" outMap["b"]="2"
		//static bool StrToMap(CStr2Map& outMap,const string& Str,string sEleSep = "&" ,string sNvSep="=" ,int emptyFlag = 0);

		static void MapToStr(CStr2Map& inMap,string& Str,string sEleSep= "&" , string sNvSep = "=" ) ;

		static void MapToStrNoEncode(CStr2Map& inMap,string& Str,string sEleSep= "&" , string sNvSep = "=") ;

		//! 把容器的内容转换成xml格式的字符串里面
		static void MapToXml(CStr2Map& inMap,string& Str,const string& enco = "GBK",const string& root = "root");

	private:
		static string EncodeString(const string& oldStr,int encodeType=1);
	};
}


#endif // !__LIBAPS_STRHELPER_HPP__