/*
 * @file        XMLMessage.cpp
 * @brief       == 
 * @author      sunu<pansunyou@gmail.com>
 *
 * @copyright   Copyright (c) 2012 xgd
*/
#include <base/XMLMessage.hpp>
#include <base/strHelper.hpp>
#include <base/deelx.hpp>

#define Z_LOG_NODE_DATA 
static void TrimRightBlank(char *pInput);

void XMLMessage::setNull( bool bNull )
{
    cfg.bIsNull = bNull;
    // Trace(eDEBUG, "this[%p][%s], parent(%p), %d", this, cfg.name.c_str(), mParent, cfg.bIsNull);
    if(mParent && cfg.bIsNull==false)
    {
        mParent->setNull(false);
    }
}

XMLMessage::XMLMessage():
mParent(NULL)
{
    mParserTrimValue = true;    // 默认启用.
    mIsVector = false;
}

XMLMessage::~XMLMessage()
{
}

void XMLMessage::checkValue()
{
}

void XMLMessage::setParserTrim(bool bSet)
{
    mParserTrimValue = bSet;
}

bool XMLMessage::getParserTrim()
{
    return mParserTrimValue;
}

bool XMLMessage::isNull()
{
    return cfg.bIsNull;
}

void XMLMessage::setError(const string&errMsg, int row, int column)
{
    mErrMsg = errMsg;
    mErrRow = row;
    mErrColumn = column;
}


const char* XMLMessage::getErrDesc()
{
    return mErrMsg.c_str();
}

int XMLMessage::getErrRow()
{
    return mErrRow;
}

int XMLMessage::getErrCol()
{
    return mErrColumn;
}

bool XMLMessage::schameValidate( private_t*private_, TiXmlElement* thisNode, NodeAttr*thisNodeAttr )
{
    // Schame检验
    if (thisNode==NULL && !thisNodeAttr->bCanNull)
    {
        string parentNode;
        if (mParent!=NULL)
            parentNode = mParent->cfg.name;
        string err = parentNode + _G2U("下的") + thisNodeAttr->name + _G2U("节点必须出现!");
        setError(err, 0, 0);
        private_->setError(err);
        return false;
    }

    if (thisNode==NULL)
        return true;
    
    return true;
}

void XMLMessage::parserNodeAttribute( TiXmlAttribute* firstAttr, map<string, string>& attrMap )
{
    for(TiXmlAttribute* a = firstAttr; a; a = a->Next())
    {
        attrMap.insert(make_pair<string,string>(a->Name(), a->Value()));
    }
}

void XMLMessage::attrToXML( string&toXML_, map<string, string>& attrMap )
{
    map<string, string>::const_iterator it = attrMap.begin();
    
    string val;
    for (;it!=attrMap.end();it++)
    {
        val = "";
        TiXmlBase::EncodeString( it->second, &val );
        toXML_ += " " + it->first + "=\"" + val + "\""; 
    }
}


bool XMLMessage::parse( private_t*private_, TiXmlAttribute* firstAttr )
{
    // 啥事都不干
    return true;
}

void XMLMessage::setNodeName( const string& str )
{
    cfg.name = str;
}

void XMLMessage::setSubnodeName()
{
}


string XMLMessage::getParentPath()
{
    if (!mParent)
    {
        return "/";
    }
    else 
    {
        if (mParent->mParent)
        {
            if (mIsVector)
                return mParent->getParentPath();
            return mParent->getParentPath() + "/" + cfg.name;
        }
        else 
            return mParent->getParentPath() + cfg.name;
    }
}


SimpleXSDType::SimpleXSDType()
{
}

SimpleXSDType::~SimpleXSDType()
{
}


const char* SimpleXSDType::c_str() const
{
    return mVal.c_str();
}


string& SimpleXSDType::str()
{
    return mVal;
}


void SimpleXSDType::toXML( string &appendToXML, int level )
{
    if (cfg.bIsNull)
        return ;

    checkValue();

    string prefix = StrHelper::repeat(_Z_SPACECHAR, level);
    string attrText = "";
    attrToXML(attrText, attrMap);
    
    appendToXML += prefix + "<"+cfg.name+attrText+">";

    string val;
    TiXmlBase::EncodeString( mVal, &val );
    appendToXML += val;

    appendToXML += "</"+cfg.name+">\n";
}


bool SimpleXSDType::parse( private_t*private_, TiXmlAttribute* firstAttr )
{
    for(TiXmlAttribute* a = firstAttr; a; a = a->Next())
    {
        if (cfg.name==a->Name())
        {
            const char*pText = a->Value();
            if (pText!=NULL)
                mVal = pText;
            checkValue();
            
            if (mParserTrimValue && pText!=NULL)
            {
                TrimRightBlank((char*)pText);
                mVal = pText;
            }
            return true;
        }
    }
    return true;
}


bool SimpleXSDType::parse( private_t*private_, TiXmlElement* thisNode )
{
    // schame校验
    if (!schameValidate(private_, thisNode, &cfg))
    {
        return false;
    }

    if (thisNode==NULL)
    {
        setNull(true);
        return true;
    }

    setNull(false);
    const char*pText = thisNode->GetText();
    if (pText!=NULL)
        mVal = pText;

    checkValue();
    if (mParserTrimValue && pText!=NULL)
    {
        TrimRightBlank((char*)pText);
        mVal = pText;
    }
    
    return true;
}


SimpleXSDType& SimpleXSDType::operator=(const SimpleXSDType&other)
{
    if (&other!=this)
    {
        mVal = other.c_str();
    }
    return *this;
}


SimpleXSDType& SimpleXSDType::operator=( const string& str )
{
    mVal = str;

    // 可选节点, 只有在值不为空的时候, 才出现.
    if (cfg.bCanNull && !mVal.empty())
    {
        setNull(false);
    }
    // 必选节点, 要向上设置父节点
    else if (!cfg.bCanNull)
    {
        setNull(false);
    }
    
    return *this;
}


SimpleXSDType& SimpleXSDType::operator+=(const string& str)
{
    mVal += str;
    return *this;
}


bool SimpleXSDType::operator==(const string& str)
{
    return mVal == str;
}


bool SimpleXSDType::operator!=(const string& str)
{
    return mVal != str;
}


size_t SimpleXSDType::length()
{
    return mVal.length();
}


size_t SimpleXSDType::size()
{
    return mVal.size();
}

void SimpleXSDType::resize ( size_t n, char c )
{
    return mVal.resize(n, c);
}

void SimpleXSDType::resize ( size_t n )
{
    return mVal.resize(n);
}

void SimpleXSDType::reserve ( size_t res_arg )
{
    return mVal.reserve(res_arg);
}

size_t SimpleXSDType::find ( const string& str, size_t pos) const
{
    return mVal.find(str, pos);
}

size_t SimpleXSDType::find ( const char* s, size_t pos, size_t n ) const
{
    return mVal.find(s, pos, n);
}

size_t SimpleXSDType::find ( const char* s, size_t pos  ) const
{
    return mVal.find(s, pos);
}

size_t SimpleXSDType::find ( char c, size_t pos ) const
{
    return mVal.find(c, pos);
}

string SimpleXSDType::substr ( size_t pos , size_t n) const
{
    return mVal.substr(pos, n);
}

SimpleXSDType::operator string()
{
    return mVal;
}

void SimpleXSDType::checkValue()
{
    try
    {
        if (cfg.xsdType==NodeAttr::eSimpleType)
            cfg.restriction.validate(mVal.c_str());
    }
    catch (Exception& e)
    {
        string err;
        err = formatStr("Node: %s, Error: %s", getParentPath().c_str(), e.what()).c_str();
        throw Exception(Z_SOURCEINFO, e.code(), err);
    }
}


void Restriction::put( string key, string val )
{
    if (key=="enumeration")
        put_enumeration(val.c_str());
    else 
        mList.insert(make_pair<string,string>(static_cast<string&&>(key),static_cast<string&&>(val)));
}

bool Restriction::get( string key, string& val, string def/*=""*/ )
{
    std::map<string,string>::const_iterator it = mList.find(key);
    if (it==mList.end())
    {
        val = def;
        return false;
    }
    
    val = it->second;
    return true;
}

void Restriction::validate( const char*val )
{
    std::map<string,string>::const_iterator it = mList.begin();
    
    for (;it!=mList.end();it++)
    {
        checkRule(it->first.c_str(), it->second.c_str(), val);
    }

    // dateTime
    // string 
    // decimal
    // date
    // time
    // boolean

    // double
    // float
    // byte
    
    // base64Binary 
    // hexBinary
    // anyURI
    // 
    string regexpS = "";

    if (baseType.find(":")!=string::npos)
        baseType = baseType.substr(baseType.find(":")+1);

    if (baseType=="dateTime")
    {
        regexpS = "^([1-9][0-9]{3}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2})(Z|[+-][0-9]{2}:[0-9]{2})?$";
    }
    else if(baseType=="date")
    {
        regexpS = "^([1-9][0-9]{3}-[0-9]{2}-[0-9]{2})(Z|[+-][0-9]{2}:[0-9]{2})?$";
    }
    else if(baseType=="time")
    {
        regexpS = "^([0-9]{2}:[0-9]{2}:[0-9]{2})(Z|[+-][0-9]{2}:[0-9]{2})?$";
    }
    else if(baseType=="boolean")
    {
        regexpS = "^(true|false))$";
    }
    else if(baseType=="decimal"||baseType=="double"||baseType=="float")
    {
        regexpS = "^([+-]?[^\\.]+\\.?[0-9]+)$";
    }
    else if (baseType=="integer")
    {
        regexpS = "^(\\d+)$";
    }
    
    if (regexpS!="")
    {
        CRegexpT <char> regexp(regexpS.c_str());
        MatchResult result = regexp.MatchExact(val);
        if (!result.IsMatched())
        {
            #ifdef Z_LOG_NODE_DATA
            Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
            #endif 
            
            logException(Z_SOURCEINFO, "5002", _G2U("Value(%s) isn't validate %s"), val, baseType.c_str());
        }
    }

}


void Restriction::checkRule( const char*key, const char*rule, const char*val )
{
/*
    length	        定义所允许的字符或者列表项目的精确数目。必须大于或等于0。
    maxLength	    定义所允许的字符或者列表项目的最大数目。必须大于或等于0。
    minLength	    定义所允许的字符或者列表项目的最小数目。必须大于或等于0。
    enumeration	    定义可接受值的一个列表
    pattern	        定义可接受的字符的精确序列。
    fractionDigits	定义所允许的最大的小数位数。必须大于等于0。
    totalDigits	    定义所允许的阿拉伯数字的精确位数。必须大于0。
    maxExclusive	定义数值的上限。所允许的值必须小于此值。
    maxInclusive	定义数值的上限。所允许的值必须小于或等于此值。
    minExclusive	定义数值的下限。所允许的值必需大于此值。
    minInclusive	定义数值的下限。所允许的值必需大于或等于此值。
    whiteSpace	    定义空白字符（换行、回车、空格以及制表符）的处理方式。
*/
    key = key==NULL?"":key;
    rule = rule==NULL?"":rule;

    if (val==NULL)
        return;

    if (strcmp(key, "length")==0)
    {
        int length = atoi(rule);
        int length2 = strlenUtf8(val);
        if (length2!=length)
        {
            #ifdef Z_LOG_NODE_DATA
            Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
            #endif 
            
            logException(Z_SOURCEINFO, "5002", _G2U("Length(%d) not equal to %d"), length2, length);
        }
    }
    else if (strcmp(key, "maxLength")==0)
    {
        int maxLength = atoi(rule);
        int length2 = strlenUtf8(val);
        if (length2>maxLength)
        {
            #ifdef Z_LOG_NODE_DATA
            Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
            #endif 
            
            logException(Z_SOURCEINFO, "5002", _G2U("Length(%d) can't larger then %d"), length2, maxLength);
        }            
    }
    else if (strcmp(key, "minLength")==0)
    {
        int minLength = atoi(rule);
        int length2 = strlenUtf8(val);
        if (length2<minLength)
        {
            #ifdef Z_LOG_NODE_DATA
            Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
            #endif 
            
            logException(Z_SOURCEINFO, "5002", _G2U("Length(%d) can't less than %d"), length2, minLength);
        }
    }
    else if (strcmp(key, "enumeration")==0)
    {
        string tmp;
        tmp = "|";
        tmp += val;
        tmp += "|";
        if (strstr(rule, tmp.c_str())==NULL)
        {
            #ifdef Z_LOG_NODE_DATA
            Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
            #endif 
            
            logException(Z_SOURCEINFO, "5002", _G2U("Value(%s) not in (%s)"), val, rule);
        }
    }
#define _Z_ENABLE_PATTERN_
#ifdef _Z_ENABLE_PATTERN_
    else if (strcmp(key, "pattern")==0)
    {
        string regexpS = "^(";
        regexpS += rule;
        regexpS += ")$";

        CRegexpT <char> regexp(regexpS.c_str());
        MatchResult result = regexp.MatchExact(val);
        if (!result.IsMatched())
        {
            #ifdef Z_LOG_NODE_DATA
            Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
            #endif 
            
            logException(Z_SOURCEINFO, "5002", _G2U("Value not valid, pattern: %s"), rule);
        }
    }
#endif
    else if (strcmp(key, "minInclusive")==0)
    {
        double minInclusive = atof(rule);
        double valD = atof(val);
        if (valD<minInclusive)
        {
            #ifdef Z_LOG_NODE_DATA
            Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
            #endif 
            
            logException(Z_SOURCEINFO, "5002", _G2U("Value(%0.2lf) can't smaller than %0.2lf"), valD, minInclusive);
        }
    }
    else if (strcmp(key, "maxInclusive")==0)
    {
        double maxInclusive = atof(rule);
        double valD = atof(val);
        if (valD>maxInclusive)
        {
            #ifdef Z_LOG_NODE_DATA
            Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
            #endif 
            logException(Z_SOURCEINFO, "5002", _G2U("Value(%0.2lf) can't larger than %0.2lf"), valD, maxInclusive);
        }
    }

    else if (strcmp(key, "minExclusive")==0)
    {
        double minExclusive = atof(rule);
        double valD = atof(val);

        if (valD>minExclusive)
        {}
        else 
        {
            #ifdef Z_LOG_NODE_DATA
            Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
            #endif 
        
            logException(Z_SOURCEINFO, "5002", _G2U("Value(%0.2lf) must larger than %0.2lf"), valD, minExclusive);
        }
    }
    else if (strcmp(key, "maxExclusive")==0)
    {
        double maxExclusive = atof(rule);
        double valD = atof(val);
        if (valD<maxExclusive)
        {}
        else 
        {
            #ifdef Z_LOG_NODE_DATA
            Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
            #endif 
        
            logException(Z_SOURCEINFO, "5002", _G2U("Value(%0.2lf) must smaller than %0.2lf"), valD, maxExclusive);
        }
    }

    else if (strcmp(key, "fractionDigits")==0)
    {
        int fractionDigits = atoi(rule);
        if (fractionDigits>=0)
        {
            const char* pos = strchr(val, '.');
            if (pos!=NULL)
            {
                int length = strlen(val) - (pos-val);
                if (length>fractionDigits)
                {
                    #ifdef Z_LOG_NODE_DATA
                    Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
                    #endif
                    
                    logException(Z_SOURCEINFO, "5002", _G2U("fraction digits(%d) must less then (%d)"), length, fractionDigits);
                }
            }
        }
    }
    else if (strcmp(key, "totalDigits")==0)
    {
        int totalDigits = atoi(rule);
        if (totalDigits>=0)
        {
            const char* pos = strchr(val, '.');
            int length = strlen(val);
            if (pos!=NULL)
                length -= 1;
            if (length>totalDigits)
            {
                #ifdef Z_LOG_NODE_DATA
                Z_LOG_X(eTRACE) << "NodeValue:[" << val << "]";
                #endif 

                logException(Z_SOURCEINFO, "5002", _G2U("total digits(%d) catn't more then (%d)"), length, totalDigits);
           }
        }
    }
    // 其它暂时不支持
    
}


void Restriction::put_enumeration( const char *val )
{
    val = val==NULL?"":val;
    std::map<string,string>::const_iterator it = mList.find("enumeration");
    if (it==mList.end())
    {
        mList.insert(make_pair<string,string>("enumeration", "|" + string(val)+ "|"));
    }
    else 
    {
        string v;
        get("enumeration", v);
        v += val;
        v += "|";
        mList["enumeration"] = v;
    }
}

void Restriction::parseFromXML( TiXmlElement* thisNode )
{
}

/// 只去除右边的ASCII字符
static void TrimRightBlank(char *pInput)
{
    int n = strlen(pInput);
    for(int pos=n-1;pos >=0 && pInput[pos]==' ';pos--)
    {
        pInput[pos] = 0;
    }
}


