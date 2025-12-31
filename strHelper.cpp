/*
* @file        strHelpercpp
* @brief       字符串工具
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#include <base/strHelper.hpp>
#include <base/toolkit.hpp>

#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <algorithm>

using namespace aps;

namespace aps
{
    int strlenUtf8(const char *s) 
    {
        int i = 0, j = 0;
        while (s[i]) {
            if ((s[i] & 0xc0) != 0x80) j++;
            i++;
        }
        return j;
    }

    string double2string(double val, string format)
    {
        char szVal[100];
        snprintf(szVal, sizeof(szVal), format.c_str(), val);
        return szVal;
    }


    string int2string(int val, string format)
    {
        char szVal[100];
        snprintf(szVal, sizeof(szVal), format.c_str(), val);
        return szVal;
    }

    std::string formatStr::str()
    {
        return mBuffer==NULL?"":mBuffer;
    }

    const char* formatStr::c_str() const
    {
        return mBuffer==NULL?"":mBuffer;
    }

    formatStr::operator const char*()
    {
        return mBuffer==NULL?"":mBuffer;
    }

    formatStr::operator char*()
    {
        return mBuffer==NULL?(char*)"":mBuffer;
    }

    formatStr::operator string()
    {
        return mBuffer==NULL?"":mBuffer;
    }





};


string StrHelper::randomString(int length)
{
    string rstr;
    //rstr.resize(length, 0);

    static char table[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i=0; i<length; i++) 
    {
        rstr += table[ (int)Toolkit::random(0, sizeof(table)-1) ];
    }

    return rstr;
}


// [0x81-0xFE][0x40-0xFE]
string StrHelper::getValidGbkString(string origGbkStr, bool bDelBadChar, char replaceBadChar)
{
    /*
    GBK字符集
    作用：它是GB2312的扩展，加入对繁体字的支持，兼容GB2312。
    位数：使用2个字节表示，可表示21886个字符。
    范围：高字节从81到FE，低字节从40到FE。

    GB2312字符集
    作用：国家简体中文字符集，兼容ASCII。
    位数：使用2个字节表示，能表示7445个符号，包括6763个汉字，几乎覆盖所有高频率汉字。
    范围：高字节从A1到F7, 低字节从A1到FE。将高字节和低字节分别加上0XA0即可得到编码。
    */

    if (origGbkStr.length()<=0)
        return origGbkStr;

    char *pBuffer = strdup(origGbkStr.c_str());
    Z_ASSERT(pBuffer!=NULL);

    // 过滤非法GBK字符
    unsigned char *pStrip = (unsigned char *)pBuffer;
    unsigned char *pStripEnd = pStrip+strlen(pBuffer);

    int leftLength = 0;
    for (; pStrip<pStripEnd;)
    {
        if ( (*pStrip)>=0x81 && (*pStrip)<=0xFE )
        {
            if ((pStrip+1)==pStripEnd) 
            {
                if (bDelBadChar)
                    *pStrip = '\0';
                else
                    *pStrip = replaceBadChar;
                break;
            }

            if ( (*(pStrip+1))>=0x40 && (*(pStrip+1))<=0xFE )
            {
                pStrip += 2;
            }
            else
            {
                if (bDelBadChar)
                {
                    leftLength = strlen((char*)pStrip)-1;
                    pStripEnd -= 1;
                    memmove(pStrip, pStrip+1, leftLength);
                }
                else
                {
                    *pStrip = replaceBadChar;
                }
                pStrip++;
            }
        }
        else
        {
            pStrip++;
        }
    }
    *pStripEnd = '\0';

    origGbkStr = pBuffer;
    free(pBuffer);
    return origGbkStr;
}

// [0x81-0xFE][0x40-0xFE]
string StrHelper::gbkSubstr(string gbkBigStr, size_t from, size_t length)
{
    string substring;
    if (gbkBigStr.length()<=0 || length<=0)
        return substring;

    if (from>=gbkBigStr.length())
        return substring;

    if ( (from+length) >= gbkBigStr.length() )
        length = gbkBigStr.length() - from;

    char *pBuffer = new char[gbkBigStr.length()+2];
    Z_ASSERT(pBuffer!=NULL);
    memset(pBuffer, 0x00, gbkBigStr.length()+2);
    memcpy(pBuffer, gbkBigStr.c_str(), gbkBigStr.length());

    // 过滤非法GBK字符
    unsigned char *pStrip = (unsigned char *)pBuffer;
    unsigned char *pStripEnd = pStrip+strlen(pBuffer);
    for (; pStrip<pStripEnd; )
    {
        if ( (*pStrip)>=0x81 && (*pStrip)<=0xFE )
        {
            if ((pStrip+1)==pStripEnd) 
            {
                *pStrip = ' ';
                break;
            }

            if ( (*(pStrip+1))>=0x40 && (*(pStrip+1))<=0xFE )
            {
                pStrip += 2;
            }
            else
            {
                *pStrip = ' '; // 非法
                pStrip++;
            }
        }
        else
        {
            pStrip++;
        }
    }

    //找到起点
    unsigned char *pStart = (unsigned char *)pBuffer;
    unsigned char *pStringEnd = (unsigned char *)pBuffer+from+length;
    unsigned char *pStringTmpStart = (unsigned char *)pBuffer+from;

    for (; (pStart<pStringTmpStart+1) && (pStart<pStringEnd);)
    {
        if (pStart==pStringTmpStart)
        {
            break;
        }

        if ( (*pStart)>=0x81 && (*pStart)<=0xFE )
        {
            if (pStart==pStringTmpStart)
            {
                break;
            }
            else if (pStart+1==pStringTmpStart)
            {
                pStart++;
                *pStart = ' ';
                break;
            }
            pStart += 2;
        }
        else
        {
            pStart++;
        }
    }

    // 查找结尾, 从pStart查到遇到非法字符
    unsigned char *pTmpEndPos = pStringEnd;
    pStringEnd = pStart;

    for (; pStringEnd<pTmpEndPos; pStringEnd++)
    {
        if ( (*pStringEnd)>=0x81 && (*pStringEnd)<=0xFE )
        {
            // 只取一个字符, 并且这个字符是非法的.
            if ((pStringEnd+1)==pTmpEndPos) 
            {
                *pStringEnd = ' ';
                continue;
            }

            // 合法的GBK初始位置
            if ( (*(pStringEnd+1))>=0x40 && (*(pStringEnd+1))<=0xFE )
            {
                pStringEnd++;
            }
            else
            {
                break; // 非法, 继续下一个字符
            }
        }
    }

    *pStringEnd = '\0';
    substring = (char*)pStart;

    delete []pBuffer;
    return substring;
}


string StrHelper::replaceLoop(string str,const string old_value, const string new_value)   
{
    string::size_type pos(string::npos);   
    pos=str.find(old_value);
    for (;pos!=string::npos; pos=str.find(old_value))
    {
        str.replace(pos,old_value.length(),new_value);   
        if ( new_value.find(old_value) !=string::npos ) 
            break;              ///防止死循环,比如 aa -> aaa
    }
    return str;
}


string StrHelper::replace(string str,const string old_value, const string new_value)   
{
    string::size_type pos(string::npos);   
    pos=str.find(old_value);
    for (;pos!=string::npos;)
    {
        str.replace(pos, old_value.length(), new_value);   
        pos=str.find(old_value, pos+new_value.length());
    }
    return str;
}

wstring StrHelper::replaceW(wstring str,const wstring old_value, const wstring new_value)   
{
    wstring::size_type pos(wstring::npos);   
    pos=str.find(old_value);
    for (;pos!=wstring::npos;)
    {
        str.replace(pos, old_value.length(), new_value);   
        pos=str.find(old_value, pos+new_value.length());
    }
    return str;
}

string StrHelper::repeat(const string& instr, size_t count)
{
    string out;
    for (size_t i=0;i<count;i++)
        out += instr;
    if (count<=0)
        return "";
    return out;
}

string StrHelper::strpad(string instr, char charToFill, size_t lastStrLength, StrpadDirect_t dir)
{
    if (instr.length()>=lastStrLength)
        return instr;

    size_t sizeToPad = lastStrLength - instr.length();

    string toAdd;
    toAdd.resize(sizeToPad, charToFill);
    
    if (dir==eStrpadDirectLeft)
        instr = toAdd + instr;
    else
        instr += toAdd;

    return instr;
}


string StrHelper::trim(string inStr, const char*char2trim)
{
    char2trim = char2trim==NULL?"\r\n \t":char2trim;
    if (inStr.size()<=0)
        return inStr;

    string::size_type pos = inStr.find_first_not_of(char2trim);
    if (pos!=string::npos)
    {
        inStr.erase(0, pos);
    }
    else
    {
        inStr.erase(0);
    }

    pos = inStr.find_last_not_of(char2trim);
    if (pos!=string::npos)
        inStr.erase(pos+1);

    return inStr;
}


string StrHelper::trimLeft(string inStr, const char*char2trim)
{
    char2trim = char2trim==NULL?"\r\n \t":char2trim;
    if (inStr.size()<=0)
        return inStr;

    string::size_type pos = inStr.find_first_not_of(char2trim);
    if (pos!=string::npos)
        inStr.erase(0, pos);
    return inStr;
}


string StrHelper::trimRight(string inStr, const char*char2trim)
{
    char2trim = char2trim==NULL?"\r\n \t":char2trim;
    if (inStr.size()<=0)
        return inStr;

    string::size_type pos = inStr.find_last_not_of(char2trim);
    if (pos!=string::npos)
        inStr.erase(pos+1);

    return inStr;
}


vector<string> &StrHelper::split(const string &s, char delim, vector<string> &elems) 
{
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

vector<string> StrHelper::split(const string &s, char delim)
{
    vector<string> elems;
    return split(s, delim, elems);
}


vector<string> StrHelper::split(const string& s, const string& delim, const bool keep_empty /*= true*/) 
{
    vector<string> result;
    if (delim.empty()) {
        result.push_back(s);
        return result;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty()) {
            result.push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}


string StrHelper::toUpper(string str)
{
    string upperStr = str;
    for(size_t i=0; i<upperStr.length(); i++)
    {
        if (upperStr[i]>='a' && upperStr[i]<='z')
            upperStr[i] = upperStr[i] - 'a' + 'A';
    }
    return upperStr;
}

//! 第一个字符大写
string StrHelper::upperFirstChar(string str)
{
    if (str=="")
        return str;
    if (str[0]>='a' && str[0]<='z')
        str[0] = str[0] - 'a' + 'A';
    return str;
}

//! 第一个字符小写
string StrHelper::lowerFirstChar(string str)
{
    if (str=="")
        return str;
    if (str[0]>='A' && str[0]<='Z')
        str[0] = str[0] + ('a' - 'A');
    return str;
}

string StrHelper::toLower(string str)
{
    string lowerStr = str;
    for(size_t i=0; i<lowerStr.length(); i++)
    {
        if (lowerStr[i]>='A' && lowerStr[i]<='Z')
            lowerStr[i] = lowerStr[i] - 'A' + 'a';
    }
    return lowerStr;
}


// 从:abc:abc的值:def:def的值:这样格式的字符串取字段的值
string StrHelper::readKeyFromExtString(string bigstring, string key)
{
    string retStr;
    if (bigstring.length()<=2||key.length()<=2)
        return retStr;

    if (bigstring.at(0)!=':')
        bigstring = ":" + bigstring;

    if (bigstring.at(bigstring.length()-1)!=':')
        bigstring += ":";

    if (key.at(0)!=':')
        key = ":" + key;
    if (key.at(key.length()-1)!=':')
        key += ":";

    string::size_type posStart = bigstring.find(key);
    if( posStart == string::npos )
        return retStr;

    posStart += key.length();
    string::size_type posEnd = bigstring.find(":", posStart);
    if( posEnd == string::npos )
    {
        retStr = bigstring.substr(posStart);
        return retStr;
    }

    retStr = bigstring.substr(posStart, posEnd-posStart);
    //retStr = replaceStrLoop(retStr, "^", ":");
    return retStr;
}

//! @brief 把数字替换成指定参数
string StrHelper::SubNumWithChar(const string& str,const int& iIndex,const int iCnt,const char& c)
{
	int iLen = str.length() ;
	if(iIndex + iCnt > iLen)
	{
		return "" ;
	}
	string result = str ;

	for(int iStart = iIndex ;iStart < iIndex + iCnt ; iStart++)
	{
		result[iStart] = c ;
	}

	return result ;
}

//! 对手机号进行掩码
/*  mode:掩码模式
0:进行134******32格式掩码[默认采用这种格式]
1:进行134****1832格式掩码
*/
string StrHelper::MobileMask(const string& mobile,int mode/* =0 */)
{
	std::string strMobile;
	if(mobile.size()!=11)
	{
		return "";
	}
	switch(mode)
	{
	case 0:
		//得到134******32格式，默认返回
		strMobile = SubNumWithChar(mobile,3,6,'*');
		break;
	case 1:
		//得到134****1832格式
		strMobile = SubNumWithChar(mobile,3,4,'*');
		break;
	default:
		strMobile = SubNumWithChar(mobile,3,6,'*');
		break;
	}
	return strMobile;
}


string StrHelper::CredIdMask(const string& credid,int mode/* =0 */)
{
	std::string strCredId;
	if(credid.size()==15 || credid.size()==18)
	{
		switch(mode)
		{
		case 0:
			//得到431121***********4格式,默认返回
			strCredId = SubNumWithChar(credid,6,credid.size()-7,'*');
			break;
		case 1:
			//得到4311211990********格式
			strCredId = SubNumWithChar(credid,credid.size()-8,8,'*');
			break;
		case 2:
			//得到431121**********34格式,默认返回
			strCredId = SubNumWithChar(credid,6,credid.size()-8,'*');
			break;
		default:
			strCredId = SubNumWithChar(credid,6,credid.size()-7,'*');
			break;
		}
	}else
	{
		return "";
	}
	return strCredId;
}

void StrHelper::ShieldCard(string &sCreId, char cSymbol/* = */)
{
	if(sCreId.size() < 6)
	{
		sCreId = string(sCreId.size(), cSymbol);
	}
	else
	{
		int iBegin = 2;
		int iEnd = 2;
		if(sCreId.size() >= 10)
		{
			iBegin = 6;
			iEnd = 2;
		}

		for(unsigned int i=iBegin; i<sCreId.size()-iEnd; i++)
		{
			sCreId[i] = cSymbol;
		}
	}
}


// 修改:Key:Val:
string StrHelper::setKeyToExtString(string &bigString, string key, string val)
{
    if (bigString.length()>=1)
        if (bigString.at(0)!=':')
            bigString = ":" + bigString;

    if (bigString.length()>=2)
        if (bigString.at(bigString.length()-1)!=':')
            bigString += ":";

    //val = replaceStrLoop(val, ":", "^");
    string secToFind = ":" + key + ":";
    string::size_type posStart = bigString.find(secToFind);
    if( posStart == string::npos )
    {
        bigString += secToFind;
        bigString += val + ":";
        return bigString;
    }
    else 
    {
        posStart += secToFind.length();
        string::size_type posEnd = bigString.find(":", posStart);
        // ***:abc:****
        if( posEnd == string::npos )
            bigString.replace(posStart, bigString.length()-posStart, val);
        else
            bigString.replace(posStart, posEnd-posStart, val);
    }

    return bigString;
}


//! 用于字符串的格式化, 类似于sprintf
formatStr::formatStr(const char*fmt ...)
{
    mBuffer = NULL;
    fmt = fmt==NULL?"":fmt;
    if( NULL == strchr( fmt, '%' ) )
    {
        int size = strlen(fmt)+1;
        mBuffer = (char*)calloc(size, sizeof(char));
        if (NULL!=mBuffer)
        {
            memcpy(mBuffer, fmt, size);
            mBuffer[size-1] = '\0';
        }
        return ;
    }

    va_list args;
    char *pBuffer = NULL;
    char *pRallocBuffer = NULL;
    size_t bufferSize = 0;
    size_t nexSize = 0;

    for (;;)
    {
        if (bufferSize == 0) 
        {
            int _size = (int) (strlen(fmt)*1.5);
            if (_size<512)
                _size = 512;
            pBuffer = (char *)malloc(_size);
            if (pBuffer == NULL) 
            {
                // 内存错误
                pBuffer = NULL;
                break;
            }
            bufferSize = _size;
        } 
        else if ((pRallocBuffer = (char *)realloc(pBuffer, nexSize)) != NULL) 
        {
            pBuffer = pRallocBuffer;
            bufferSize = nexSize;
        } 
        else
        {
            pBuffer = NULL;
            break;
        }

        va_start(args, fmt);
        int realSize = vsnprintf(pBuffer, bufferSize, fmt, args);
        va_end(args);

        if (realSize == -1)
        {
            // 只能瞎猜大小 
            nexSize = bufferSize * 2;
        } 
        else if (realSize == (int)bufferSize)
        {
            // 被截断, 大小亦不知道
            nexSize = bufferSize * 2;
        } 
        else if (realSize > (int)bufferSize) 
        {
            // 这个还好, 已经知道需要多大内存
            nexSize = realSize + 2;
        } 
        else if (realSize == (int)bufferSize - 1)
        {
            // 在用些系统上不明确(不明白...)
            nexSize = bufferSize * 2;
        } 
        else
        {
            // 这样最好
            break;
        }
    }

    if (pBuffer!=NULL)
    {
        mBuffer = pBuffer;
    }
}

formatStr::~formatStr()
{
    if (mBuffer!=NULL)
    {
        free(mBuffer);
        mBuffer = NULL;
    }
}


formatStr& formatStr::operator=( const formatStr& other )
{
    if (&other != this)
    {
        if (other.mBuffer!=NULL)
        {
            if (mBuffer!=NULL)
                free(mBuffer);
            mBuffer = (char*)calloc(strlen(other.mBuffer)+2, 1);
            strcpy(mBuffer, other.mBuffer);
        }
    }
    return *this;
}


formatStr::formatStr( const formatStr& other )
{
    mBuffer = NULL;
    if (other.mBuffer!=NULL)
    {
        mBuffer = (char*)calloc(strlen(other.mBuffer)+2, 1);
        strcpy(mBuffer, other.mBuffer);
    }
}




KVString::KVString()
{
    mBigString = ";";
}


KVString::KVString( const string& bigString )
{
    parse(bigString);
}

KVString::~KVString()
{
}


std::string KVString::toString()
{
    return mBigString;
}

KVString& KVString::removeKey( const string&key )
{
    if (mBigString.length()>=1)
        if (mBigString.at(0)!=';')
            mBigString = ";" + mBigString;

    if (mBigString.length()>=2)
        if (mBigString.at(mBigString.length()-1)!=';')
            mBigString += ";";

    string secToFind = ";" + key + "=";
    string::size_type posStart = mBigString.find(secToFind);
    if( posStart == string::npos )
    {
        return *this;
    }
    else 
    {
        posStart += secToFind.length();

        // 跳过\;
        string::size_type posStartV0 = posStart;
        string::size_type posTmp = mBigString.find("\\;", posStart);
        for (; posTmp!= string::npos ;)
        {
            posStart += 2;
            posTmp = mBigString.find("\\;", posStart);
        }

        string::size_type posEnd = mBigString.find(";", posStart);
        if( posEnd == string::npos )
            mBigString.replace(posStart - secToFind.length() , mBigString.length()-posStartV0, "");
        else 
            mBigString.replace(posStart - secToFind.length(), posEnd-posStartV0, "");
    }
    return *this;
}


KVString& KVString::setDouble( const string&key, double val )
{
    setString(key, double2string(val));
    return *this;
}


KVString& KVString::setInt( const string&key, int val )
{
    setString(key, int2string(val));
    return *this;
}


KVString& KVString::setString( const string&key, const string& val )
{
    if (mBigString.length()>=1)
        if (mBigString.at(0)!=';')
            mBigString = ";" + mBigString;

    if (mBigString.length()>=2)
        if (mBigString.at(mBigString.length()-1)!=';')
            mBigString += ";";

    string val_=val;
    processVal(val_);

    string secToFind = ";" + key + "=";
    string::size_type posStart = mBigString.find(secToFind);
    if( posStart == string::npos )
    {
        mBigString += key + "=";
        mBigString += val_ + ";";
        return *this;
    }
    else 
    {
        posStart += secToFind.length();

        // 跳过\;
        string::size_type posStartV0 = posStart;
        string::size_type posTmp = mBigString.find("\\;", posStart);
        for (; posTmp!= string::npos ;)
        {
            posStart += 2;
            posTmp = mBigString.find("\\;", posStart);
        }

        string::size_type posEnd = mBigString.find(";", posStart);
        if( posEnd == string::npos )
            mBigString.replace(posStartV0, mBigString.length()-posStartV0, val_);
        else 
            mBigString.replace(posStartV0, posEnd-posStartV0, val_);
    }
    return *this;
}


double KVString::getDouble( const string&key, double defVal/*=0*/ )
{
    string s = getStr(key, double2string(defVal));
    return atof(s.c_str());
}


int KVString::getInt( const string&key, int defVal/*=0*/ )
{
    string s = getStr(key, int2string(defVal));
    return atoi(s.c_str());
}


std::string KVString::getString( const string&key, string defVal/*=""*/ )
{
    string s = getStr(key, defVal);
    unProcessVal(s);
    return s;
}

KVString& KVString::parse( const string& bigString )
{
    mBigString = bigString;

    if (mBigString.length()>0)
    {
        if (mBigString.at(0)!=';')
            mBigString = ";" + mBigString;

        if (mBigString.at(mBigString.length()-1)!=';')
            mBigString += ";";
    }
    return *this;
}

void KVString::dump()
{
    printf("%s\n", mBigString.c_str());
}

std::string KVString::getStr( const string&key, string defVal )
{
    string retStr = defVal;
    string key_ = key;

    if (mBigString.length()<=2)
        return retStr;

    if (mBigString.at(0)!=';')
        mBigString = ";" + mBigString;

    if (mBigString.at(mBigString.length()-1)!=';')
        mBigString += ";";

    key_ = ";" + key_ + "=";
    string::size_type posStart = mBigString.find(key_);
    if( posStart == string::npos )
        return retStr;

    posStart += key_.length();

    // 跳过\;
    string::size_type posStartV0 = posStart;

    string::size_type posTmp = mBigString.find("\\;", posStart);
    string::size_type posEnd = mBigString.find(";", posStart);

    if (posTmp!= string::npos && posEnd==posTmp+1)
    {
        for (; posTmp!= string::npos && posEnd==posTmp+1 ;)
        {
            posStart = posTmp + 2;
            posTmp = mBigString.find("\\;", posStart);
            posEnd = mBigString.find(";", posStart);
        }
    }

    posEnd = mBigString.find(";", posStart);
    if( posEnd == string::npos )
    {
        retStr = mBigString.substr(posStartV0);
        return retStr;
    }

    retStr = mBigString.substr(posStartV0, posEnd-posStartV0);
    return retStr;
}

void KVString::processVal( string&key )
{
    if (key.length()<=0)
        return ;

    if (key.find(";")!=string::npos)
    {
        key = "(hex)" + Toolkit::bin2hex(key.c_str(), key.length(), false);
    }

}

void KVString::unProcessVal( string&key )
{
    if (key.length()>=7)
    {
        if (key.substr(0, 5)=="(hex)")
        {
            string s = key.substr(5);
            if (s.length()%2==0)
            {
                char *buffer = new char[key.length()+1];
                memset(buffer, 0x00, key.length()+1);
                Toolkit::hex2bin(s, buffer, key.length()+1);
                key = buffer;
                delete buffer;
            }
        }
    }
}

int CStr2MapTool::String2Vector(const string & sSrc,const string & sSep,std::vector<std::string> & vStr)
{
	vStr.clear();
	std::string::size_type iSepLen = sSep.size();
	std::string::size_type iStart = 0;

	std::string::size_type iTmp;
	for(iTmp = sSrc.find(sSep);
		iTmp != std::string::npos;
		iTmp = sSrc.find(sSep, iStart))
	{
		vStr.push_back(sSrc.substr(iStart,iTmp - iStart));
		iStart = iTmp + iSepLen;
	}
	vStr.push_back(sSrc.substr(iStart));

	return 0;
}

void CStr2MapTool::MapCpy(const CStr2Map& src,CStr2Map& dest)
{
	CStr2Map::const_iterator iter = src.begin();
	CStr2Map::const_iterator iter_end = src.end();
	while(iter != iter_end)
	{
		dest[iter->first] = iter->second;
		++iter;
	}
}
/*
bool CStr2MapTool::StrToMap(CStr2Map& outMap,const string& Str,string sEleSep ,string sNvSep,int emptyFlag )
{
	CStrVector vEle, vNv;
	String2Vector(Str,sEleSep,vEle) ;

	for(unsigned int i = 0; i< vEle.size();i++)
	{
		String2Vector(vEle[i],sNvSep,vNv) ;
		if(vNv.size() == 1 && emptyFlag)
		{
			outMap[vNv[0]] = "" ;
		}
		else if(vNv.size() == 2)
		{
			outMap[vNv[0]] = DecodeString(vNv[1]) ;
		}
	}
	if(outMap.size() == 0)
	{
		return false ;
	}  
	return true;    
}*/


/**
*字符转换成16进制
*@param c 需要转换的字符
*return string 转换完成的字符串
*/
static string CharToHex(const char& c)
{
	string result;
	char first, second;

	first = (c & 0xF0) / 16;
	first += first > 9 ? 'A' - 10 : '0';
	second = c & 0x0F;
	second += second > 9 ? 'A' - 10 : '0';

	result.append(1, first);
	result.append(1, second);

	return result;    
}
string CStr2MapTool::EncodeString(const string & oldStr,int encodeType)
{

	string result;
	string::const_iterator iter ;
	//几种编码中不用进行编码的字符
	string escapeStr     = "*+-./@" ;
	string uriStr         = "!#$&'()*+,-./:;=?@_~" ;
	string uriCompent     =  "!'()*-._~" ;

	bool bIsEncode = true  ;

	for(iter = oldStr.begin(); iter != oldStr.end(); ++iter) 
	{
		if(isalnum(*iter))
		{
			result.append(1, *iter);
			continue ;
		}
		switch(encodeType)
		{
		case 1:
			if(uriCompent.find(*iter) != string::npos)
			{
				bIsEncode = false ;
			}
			break;
		case 2:
			if(uriStr.find(*iter) != string::npos)
			{
				bIsEncode = false ;
			}                 
			break;
		case 3:
			if(escapeStr.find(*iter) != string::npos)
			{
				bIsEncode = false ;
			}                 
			break;         
		default:;
		}
		if((unsigned char)(*iter) > 127 || !bIsEncode)
		{
			result.append(1, *iter);
		}
		else
		{
			result.append(1, '%');
			result.append(CharToHex(*iter));
		}
		bIsEncode = true ;

	}

	return result;
}

void CStr2MapTool::MapToStr(CStr2Map& inMap,string& Str,string sEleSep/* =  */, string sNvSep /* = */ )
{

	CStr2Map::iterator iter         = inMap.begin();
	CStr2Map::iterator iter_end     = inMap.end();

	Str="";
	if(inMap.size() == 0)
	{
		return ;
	}
	Str += iter->first + sNvSep + EncodeString(iter->second) ;
	iter++ ;
	while(iter != iter_end)
	{
		Str += sEleSep + iter->first + sNvSep + EncodeString(iter->second);
		iter++ ;
	}
}

void CStr2MapTool::MapToStrNoEncode(CStr2Map& inMap,string& Str,string sEleSep/* =  */, string sNvSep /* = */ )
{
	CStr2Map::iterator iter      = inMap.begin();
	CStr2Map::iterator iter_end  = inMap.end();

	Str="";
	if(inMap.size() == 0)
	{
		return ;
	}
	Str += iter->first + sNvSep + iter->second;
	iter++ ;
	while(iter != iter_end)
	{
		Str += sEleSep + iter->first + sNvSep + iter->second;
		iter++ ;
	}
}

void CStr2MapTool::MapToXml(CStr2Map& inMap,string& Str,const string& enco /* =  */,const string& root /* = */ )
{
	stringstream sstr;
	sstr << "<?xml version=\"1.0\" encoding=\"" << enco << "\" ?>";
	sstr << "<" << root << ">";
	CStr2Map::iterator iter = inMap.begin();
	CStr2Map::iterator iter_end = inMap.end();
	if(inMap.size() != 0)
	{
		while(iter != iter_end)
		{
			sstr << "<" << iter->first << ">";
			sstr << iter->second;
			sstr << "</" << iter->first << ">";
			++iter;
		}
	}
	sstr << "</" << root << ">";
	Str = sstr.str();
}