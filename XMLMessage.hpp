/*
 * @file        XMLMessage.cpp
 * @brief       XML解析工具
 *
 * @author      潘孙友<pansunyou@gmail.com>
 *
 * @copyright   Copyright (c) 2012 xgd
*/
#ifndef __XMLMESSAGE_HPP__
#define __XMLMESSAGE_HPP__

#ifdef WIN32
#pragma warning( disable : 4251 )
#endif 

#include <base/tinyxml.hpp>
#include <base/logger.hpp>
#include <map>
#include <vector>

using namespace std;
using namespace aps;
using namespace aps::tinyxml;

#define _Z_SPACECHAR "   "

/// @brief 内部使用(待改造)
struct LIBAPS_API private_t
{
public:
    private_t()
    {
        pXMLDoc = new TiXmlDocument();
        pCurNode = NULL;
    }

    ~private_t()
    {
        delete pXMLDoc;
        pXMLDoc = NULL;
    }

    void setError(const string& er)
    {
        mErrorMsg = er;
    }
    string mErrorMsg;

    string xml;
    TiXmlDocument* pXMLDoc;
    TiXmlElement* pCurNode;
};

const int kUnbounded  = -1;
#define Z_XSD_MAXOCCURS_UNBOUNDED -1


/// @brief 简单类型限定
class LIBAPS_API Restriction
{
public:
    void validate(const char*val);
    void checkRule(const char*key, const char*rule, const char*val);

    void put(string key, string val);
    bool get(string key, string& val, string def="");

    string baseType;
private:
    void parseFromXML(TiXmlElement* thisNode);
    void put_enumeration(const char *val);
    std::map<string, string> mList;
};


/// @brief 节点属性
class LIBAPS_API NodeAttr
{
public:

    enum XsdType
    {
        eSimpleType=0,
        eSimpleContent=1,
        eComplexContent=2
    };
    
    Restriction restriction;
    
    XsdType xsdType;
    string name;
    int maxOccurs;
    int minOccurs;
    bool bIsNull;
    bool bCanNull;

    NodeAttr()
    {
        xsdType = eSimpleType;
        maxOccurs = 1;
        minOccurs = 1;
        bIsNull = false;
        bCanNull = true;
   }

};


/// @brief XML节点基类
class LIBAPS_API XMLMessage
{
public:
    XMLMessage();
    virtual ~XMLMessage();

    bool schameValidate(private_t*, TiXmlElement* thisNode, NodeAttr*thisNodeAttr);

    virtual void setNull(bool bNull);
    virtual bool isNull();
    virtual void toXML(string &appendToXML, int level) = 0;
    virtual bool parse(private_t*private_, TiXmlElement* thisNode) = 0;
    virtual bool parse(private_t*private_, TiXmlAttribute* firstAttr);
    virtual void checkValue();

    /// @brief 为了跟踪节点路径.
    string getParentPath();
    void setParserTrim(bool bSet);
    bool getParserTrim();

public:
    NodeAttr cfg;

    virtual void setSubnodeName();
    void setNodeName(const string& str);

    void setError(const string&errMsg, int row, int column);
    const char* getErrDesc();
    int getErrRow();
    int getErrCol();

public:
    map<string, string> attrMap;
    void attrToXML(string&toXML_, map<string, string>& attrMap);
    void parserNodeAttribute(TiXmlAttribute* firstAttr, map<string, string>& attrMap);
    void parserXML(private_t*private_, map<string, string*>& mbList1, map<string, XMLMessage*>& mbList);

    string mErrMsg;
    int mErrRow;
    int mErrColumn;
    bool mParserTrimValue;      // 解析时去空格
    bool mIsVector;

public:
    XMLMessage *mParent;
};


/// @brief XML简单值类型
class LIBAPS_API SimpleXSDType : public XMLMessage
{
public:
    SimpleXSDType();
    virtual ~SimpleXSDType();
    virtual void toXML(string &appendToXML, int level);
    virtual bool parse(private_t*private_, TiXmlElement* thisNode);
    virtual bool parse( private_t*private_, TiXmlAttribute* firstAttr );
    void checkValue();

public:
    SimpleXSDType& operator=(const SimpleXSDType&other);

    // 以下函数用来模拟std::string
    SimpleXSDType& operator=(const string& str);
    SimpleXSDType& operator+=(const string& str);
    bool operator==(const string& str);
    bool operator!=(const string& str);
    operator string();
    const char*c_str() const;
    string& str();
    size_t length();
    size_t size();
    void resize ( size_t n, char c );
    void resize ( size_t n );
    void reserve ( size_t res_arg=0 );
    size_t find ( const string& str, size_t pos = 0 ) const;
    size_t find ( const char* s, size_t pos, size_t n ) const;
    size_t find ( const char* s, size_t pos = 0 ) const;
    size_t find ( char c, size_t pos = 0 ) const;
    std::string substr ( size_t pos = 0, size_t n = std::string::npos ) const;

public:
    std::string mVal;
};


/// @brief XML循环节点
template <typename T>
class LIBAPS_API AnyVector : public XMLMessage
{
public:
    AnyVector()
    {
        mIsVector = true;
    }
    
    virtual ~AnyVector()
    {
        clear();
    }

    T* get(size_t index0)
    {
        T*pNode = NULL;
        if (index0 >= 0 && index0 < mItems.size())
            pNode = mItems[index0];
        else 
            logException(Z_SOURCEINFO, "5002", "index %d out of range[0,%d]", index0, mItems.size());
        return pNode;
    }
    
    T* insert()
    {
        if (cfg.maxOccurs!=Z_XSD_MAXOCCURS_UNBOUNDED && ((int)mItems.size()+1>cfg.maxOccurs) )
            logException(Z_SOURCEINFO, "5002", "Sizeof %s exceed maxOccurs(%d)", cfg.name.c_str(), mItems.size()+1, cfg.maxOccurs);

        T*pNode = new T();
        
        pNode->cfg.name = cfg.name;
        pNode->mParent = this;
        pNode->setSubnodeName();
        
        mItems.push_back(pNode);
        return pNode;
    }

    size_t size()
    {
        return mItems.size();
    }

    void remove(size_t index0)
    {
        if (index0>0 && index0< mItems.size())
        {
            mItems.erase(mItems.begin()+index0);
            delete (mItems.begin()+index0);
        }
    }

    void clear()
    {
        for (size_t i=0; i<mItems.size();i++)
            delete mItems[i];
        mItems.clear();
    }

    bool parse(private_t*private_, TiXmlElement* thisNode)
    {
        TiXmlElement* pNode = thisNode;
        size_t index0 = 0;
        for (;pNode;pNode=pNode->NextSiblingElement())
        {
            if (pNode->Value()!=cfg.name)
                continue;

            if (cfg.maxOccurs!=Z_XSD_MAXOCCURS_UNBOUNDED && ( (int)index0+1>cfg.maxOccurs) )
                logException(Z_SOURCEINFO, "5002", "Sizeof %s(%d) exceed maxOccurs(%d)", cfg.name.c_str(), index0+1, cfg.maxOccurs);

            T*p = insert();
            p->cfg.bCanNull = false;
            p->cfg.name = cfg.name;
            p->cfg.maxOccurs = kUnbounded;
            p->cfg.minOccurs = 1;
            if (!p->parse(private_, pNode))
                return false;
            
            index0++;
        }
        
        if ((int)index0 < cfg.minOccurs)
            logException(Z_SOURCEINFO, "5002", "Sizeof %s(%d) less then minOccurs(%d)", cfg.name.c_str(), index0, cfg.minOccurs);

        return true;
    }


    void toXML(string &appendToXML, int level)
    {
        if (cfg.maxOccurs!=Z_XSD_MAXOCCURS_UNBOUNDED && ((int)mItems.size()>cfg.maxOccurs) )
            logException(Z_SOURCEINFO, "5002", "Sizeof %s(%d) exceed maxOccurs(%d)", cfg.name.c_str(), mItems.size(), cfg.maxOccurs);
        
        if ((int)mItems.size() < cfg.minOccurs)
            logException(Z_SOURCEINFO, "5002", "Sizeof %s(%d) less then minOccurs(%d)", cfg.name.c_str(), mItems.size(), cfg.minOccurs);
        
        for (size_t i=0; i<mItems.size(); i++)
        {
            (mItems[i])->toXML(appendToXML, level);
        }

    }

private:
    vector<T *> mItems;
};

#endif //!__XMLMESSAGE_HPP__

