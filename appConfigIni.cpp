/*
 * @file        appConfigIni.cpp
 * @brief       配置文件类
 *
 * @author      leize<leize@xgd.com>
 * @date        2017/09/01
 * @copyright   Copyright (c) 2021 xgd 
*/
#include <base/appConfigIni.hpp>
#include <base/file.hpp>
#include <base/strHelper.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <fstream>

using namespace aps;
using namespace std;

AppConfigIni::AppConfigIni()
{
}

AppConfigIni::AppConfigIni(string path):
AppConfig(path)
{
    load();
}


AppConfigIni::~AppConfigIni()
{
}


bool AppConfigIni::load()
{
    mSections.clear();
    ifstream iniFile;
    iniFile.open(path().c_str(), ifstream::in);
    if (!iniFile)
    {
        fprintf(stderr, "fail to open file[%s]!", path().c_str());
        return false;
    }

    string  strline;
    string  strSection;
    KeyValues_t section;

    while (getline(iniFile, strline))
    {
        strline = StrHelper::trim(strline);
        if (strline.empty())
            continue;

        if (strline[0]=='#'||strline[0]==';')
            continue;

        if (strline[0]=='[')
        {
            if (!strSection.empty())
            {
				pair<string, KeyValues_t> ps = pair<string, KeyValues_t>(strSection, section);
                mSections.insert(ps);
                section.clear();
            }

            string::size_type endPos = strline.rfind(']');
            if ( endPos != string::npos && endPos>0)
            {
                strSection = strline.substr(1 ,endPos-1);
            }
            continue;
        }

        string::size_type keyPos = strline.find('=');
        if ( keyPos != string::npos)
        {
            string strKey = StrHelper::trim(strline.substr(0,keyPos));
            string strValue = StrHelper::trim(strline.substr(keyPos+1,strline.size()-keyPos));
            if (strKey.empty())
                continue;
			pair<string,string> ps = pair<string,string>(strKey, strValue);
            section.insert(ps);
        }
    }

    if (!strSection.empty())
    {
		pair<string, KeyValues_t> ps = pair<string, KeyValues_t>(strSection, section);
        mSections.insert(ps);
        section.clear();
    }

    iniFile.close();

    try
    {
        FileInfo fi(path());
        mLastLoadTime = fi.mtime();
    }
    catch (...)
    { }

    return true;
}


void AppConfigIni::save()
{
    if (mConfigPath!="")
        saveAs(mConfigPath);

    return;
}


bool AppConfigIni::saveAs(string path)
{
    FILE *fp = fopen(path.c_str(), "wb");
    if (!fp)
        return false;

    auto it = mSections.begin();
    for (;it!=mSections.end(); it++)
    {
        string secName = it->first;
        KeyValues_t& kvs = it->second;

        fprintf(fp, "[%s]\r\n", secName.c_str());

        auto it2 = kvs.begin();
        for (;it2!=kvs.end(); it2++)
        {
            fprintf(fp, "%s=%s\r\n", it2->first.c_str(), it2->second.c_str());
        }

        fprintf(fp, "\r\n");
    }

    fclose(fp);
    return true;
}


bool AppConfigIni::isSourceModified()
{
    Datetime fileUpdate;
    try
    {
        FileInfo fi(path());
        fileUpdate = fi.mtime();
    }
    catch (...)
    { }

    if (mLastLoadTime<fileUpdate)
        return true;

    return false;
}

