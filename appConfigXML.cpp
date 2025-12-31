/*
 * @file        appConfigXML.cpp
 * @brief       配置文件(XML为载体)
 *
 * @author      leize<leize@xgd.com>
 * @date        2017/09/01
 * @copyright   Copyright (c) 2021 xgd 
*/
#include <base/appConfigXML.hpp>
#include <base/file.hpp>
#include <base/strHelper.hpp>
#include <base/logger.hpp>
#include <base/tinyxml.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <fstream>
#include <memory>

using namespace std;
using namespace aps;
using namespace aps::tinyxml;

AppConfigXML::AppConfigXML()
{}

AppConfigXML::AppConfigXML(string path):AppConfig(path)
{
	load();
}

AppConfigXML::~AppConfigXML()
{
}

bool AppConfigXML::load()
{
    //创建一个XML的文档对象。
    unique_ptr<TiXmlDocument> myDocument(new TiXmlDocument());
    if (!myDocument->LoadFile(path().c_str()))
    {
        Z_LOG_X(eERROR) << "LoadXML fail :" << myDocument->ErrorDesc();
        return false;
    }
    //获得根元素，即ConfigFile
    TiXmlElement *pRootElement = myDocument->RootElement();
    if (pRootElement==NULL)
    {
        Z_LOG_X(eERROR) << "LoadXML fail, No RootElement: " << myDocument->ErrorDesc();
        return false;
    }

    KeyValues_t section;
    TiXmlElement* pSectionElement = pRootElement->FirstChildElement();
    while ( pSectionElement != NULL )
    {
        string strSection = StrHelper::trim(pSectionElement->Value());
        if (strSection=="Section")
        {
            string strSectionName = "";
            for(TiXmlAttribute* a = pSectionElement->FirstAttribute(); a; a = a->Next())
            {
                std::string name(a->Name());
                if(name == "name")
                {
                    const char*pT = a->Value();
                    if (pT!=NULL)
                        strSectionName = StrHelper::trim(pT);
                    break;
                }
            }

            if (strSectionName!="")
            {
                section.clear();
                TiXmlElement* pOptionElement = pSectionElement->FirstChildElement();
                while ( pOptionElement != NULL )
                {
                    string strOption = StrHelper::trim(pOptionElement->Value());
                    if (strOption=="Option")
                    {
                        string name_ = "";
                        for(TiXmlAttribute* a = pOptionElement->FirstAttribute(); a; a = a->Next())
                        {
                            std::string name(a->Name());
                            if(name == "name")
                            {
                                const char*pT = a->Value();
                                if (pT!=NULL)
                                    name_ = StrHelper::trim(pT);
                                break;
                            }
                        }

                        if (name_!="")
                        {
                            const char*pTxt =  pOptionElement->GetText();
                            if (pTxt!=NULL)
                            {
                                if (!strSection.empty())
                                {
									pair<string,string> ps = pair<string,string>(name_, StrHelper::trim(pTxt));
                                    section.insert(ps);
                                }
                            }
                        }
                    }
                    pOptionElement = pOptionElement->NextSiblingElement();
                }

				pair<string, KeyValues_t> ps1 = pair<string, KeyValues_t>(strSectionName, section);
                mSections.insert(ps1);
                section.clear();
            }
        }

        pSectionElement = pSectionElement->NextSiblingElement();
    }

    try
    {
        FileInfo fi(path());
        mLastLoadTime = fi.mtime();
    }
    catch (...)
    { }

    return true;
}


void AppConfigXML::save()
{
    return;
}

bool AppConfigXML::saveAs(string path)
{
    return false;
}

bool AppConfigXML::isSourceModified()
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

std::string AppConfigXML::toString()
{
    // 创建一个XML的文档对象。
    TiXmlDocument *XMLDoc = new TiXmlDocument();

    // 最先增加一个头部声明
    TiXmlDeclaration *pDeclearation = new TiXmlDeclaration( "1.0", "utf-8", "");
    XMLDoc->LinkEndChild(pDeclearation);
    
    string txt = "Update at" + Datetime::now().toString();
    TiXmlComment *pComm = new TiXmlComment(txt.c_str());
    XMLDoc->LinkEndChild(pComm);
    
    //创建一个根元素并连接。
    TiXmlElement *RootElement = new TiXmlElement("ConfigFile");
    XMLDoc->LinkEndChild(RootElement);
    
    auto it = mSections.begin();
    if (it!=mSections.end())
    {
        for (;it!=mSections.end(); it++)
        {
            KeyValues_t _section =  it->second;

            TiXmlElement *Section = new TiXmlElement("Section");
            Section->SetAttribute("name", it->first);
            
            for (auto secIter = _section.begin(); secIter!=_section.end(); secIter++ )
            {
                TiXmlElement *Option  = new TiXmlElement("Option");
                Option->SetAttribute("name", secIter->first);
                TiXmlText *Txt = new TiXmlText(secIter->second.c_str());

                Option->LinkEndChild(Txt);
                Section->LinkEndChild(Option);
            }

            XMLDoc->LinkEndChild(Section);
        }
    }
    
    TiXmlPrinter printer; 
    XMLDoc->Accept(&printer); 

    string vs = printer.CStr();
    delete XMLDoc;
    
    return vs;
}
