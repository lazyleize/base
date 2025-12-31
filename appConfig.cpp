/*
 * @file        appConfig.cpp
 * @brief       配置文件类
 *
 * @author      leize<leize@xgd.com>
 * @date        2017/09/01
 * @copyright   Copyright (c) 2021 xgd 
*/
#include <base/appConfig.hpp>
#include <base/strHelper.hpp>
#include <base/logger.hpp>
#include <cstring>
#include <cstdlib>

using namespace aps;
using namespace std;

AppConfig::AppConfig()
{}

AppConfig::AppConfig(string path):mConfigPath(path)
{}

AppConfig::~AppConfig()
{}

string AppConfig::readString(string section,string key,string def)
{
	Sections_t::iterator findSection = mSections.find(section);
	if(findSection==mSections.end())
		return def;

	KeyValues_t::iterator findString = findSection->second.find(key);
	if(findString==findSection->second.end())
		return def;
	return findString->second;
}

bool AppConfig::isSectionExists(string section)
{
	Sections_t::iterator findSection = mSections.find(section);
	if(findSection==mSections.end())
		return false;
	return true;
}

bool AppConfig::isKeyExists(string section, string key)
{
	Sections_t::iterator findSection = mSections.find(section);
	if(findSection==mSections.end())
		return false;
	return true;
}

double AppConfig::readDouble(string section, string key, double def)
{
	if(!isKeyExists(section,key))
		return def;
	else
	{
		string Val = readString(section,key);
		return atof(Val.c_str());
	}
}

int AppConfig::readInt(string section, string key, int def)
{
    if (!isKeyExists(section, key))
        return def;
    else
    {
        string Val = readString (section, key);
        return atoi(Val.c_str());
    }
}


bool AppConfig::readBool( string section, string key, bool def/*=false*/ )
{
    if (!isKeyExists(section, key))
        return def;
    else
    {
        string Val = readString (section, key);
        Val = StrHelper::toLower(Val);
        if (Val=="true"||Val==""||Val=="0")
            return false;
        else 
            return true;
    }
}

void AppConfig::dump(FILE*fp)
{
    if (fp==NULL)
        fp = stderr;
    auto it = mSections.begin();
    for (;it!=mSections.end(); it++)
    {
        KeyValues_t _section =  it->second;
        fprintf(fp, "[%s]\n", it->first.c_str());
        //cout<<"["<<it->first<<"]\r\n";
        size_t count = 0;
        for (auto secIter = _section.begin(); secIter!=_section.end(); secIter++ )
        {
            //cout<<secIter->first<<"="<<secIter->second<<"\r\n";
            fprintf(fp, "%s=%s\n", secIter->first.c_str(), secIter->second.c_str());
            count ++;
            if (count==_section.size())
                fprintf(fp, "\n");
        }
    }
}



void AppConfig::setKeyValue( const string& sectionStr, const string& key, string val )
{
    Sections_t::iterator findSection = mSections.find(sectionStr);
    if (findSection==mSections.end())
    {
        KeyValues_t section;
		pair<string,string> ps = pair<string,string>(key, val);
        section.insert(ps);
		pair<string, KeyValues_t> ps1 = pair<string, KeyValues_t>(sectionStr, section);
        mSections.insert(ps1);
    }
    else 
    {
        KeyValues_t::iterator findString = findSection->second.find(key);
        if (findString==findSection->second.end())
        {
			pair<string,string> ps = pair<string,string>(key, val);
            findSection->second.insert(ps);
        }
        else
        {
            findSection->second[key]=val;
        }
    }
}

void AppConfig::setKeyValue( const string& section, const string& key, int val )
{
    setKeyValue(section, key, int2string(val));
}

void AppConfig::setKeyValue( const string& section, const string& key, double val )
{
    setKeyValue(section, key, double2string(val));
}

void aps::AppConfig::dumpLog()
{
    Z_LOG(eINFO) << "File: "<< mConfigPath << ", Loaded At:"<< mLastLoadTime.toString() << "]-"; 

    auto it = mSections.begin();

    if (it==mSections.end())
    {
        Z_LOG(eINFO) << "Empty config file"; 
    }
    else
    {
        for (;it!=mSections.end(); it++)
        {
            KeyValues_t _section =  it->second;
            Z_LOG(eINFO) << "[" << it->first  << "]"; 
            size_t count = 0;
            for (auto secIter = _section.begin(); secIter!=_section.end(); secIter++ )
            {
                Z_LOG(eINFO) << secIter->first << "=" << secIter->second; 
                count ++;
            }
        }
    }

}

string aps::AppConfig::path()
{
    return mConfigPath;
}

void aps::AppConfig::setPath( string path_ )
{
    mConfigPath = path_;
}


std::string aps::AppConfig::toString()
{
    string vs;
    auto it = mSections.begin();

    if (it!=mSections.end())
    {
        for (;it!=mSections.end(); it++)
        {
            KeyValues_t _section =  it->second;
            vs += "[" + it->first + "]\n"; 
            size_t count = 0;
            for (auto secIter = _section.begin(); secIter!=_section.end(); secIter++ )
            {
                vs += secIter->first + "=" + secIter->second + "\n"; 
                count ++;
            }

            vs += "\n";
        }
    }

    return vs;
}


void aps::AppConfig::clear()
{
    mSections.clear();
}


void AppConfig::removeSection( const string& section )
{
    Sections_t::iterator findSection = mSections.find(section);
    if (findSection!=mSections.end())
        mSections.erase(findSection);
}


void AppConfig::removeKey( const string& section, const string& key )
{
    Sections_t::iterator findSection = mSections.find(section);
    if (findSection==mSections.end())
        return;

    KeyValues_t::iterator findString = findSection->second.find(key);
    if (findString!=findSection->second.end())
        findSection->second.erase(findString);
}


//! 读取整个Section
void AppConfig::getSection(const string&sectionName, KeyValues_t& section)
{
    Sections_t::iterator findSection = mSections.find(sectionName);
    if (findSection==mSections.end())
        return;

    section = findSection->second;
}

//! 读取所有Section
void AppConfig::getAllSection(Sections_t& sections)
{
    sections = mSections;
}
