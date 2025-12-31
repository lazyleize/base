/*
 * @file    serverApplication.cpp
 * @brief   服务程序基类
 *
 * @author  leize<leize@xgd.com>
*/

#include <base/serverApplication.hpp>
#include <base/logger.hpp>

#include <cstring>
#include <cstdio>

using namespace aps;
using namespace std;

ServerApplication::ServerApplication(int argc,char**argv):ConsoleApplication(argc,argv)
{
	mIsStopped = true;
	mIsFinished = false;
}

void ServerApplication::stop()
{
	mIsStopped = true;
	return;
}

bool ServerApplication::isStopped()
{
	return mIsStopped;
}

void ServerApplication::setStopped(bool v)
{
	mIsStopped = v;
}

void ServerApplication::setFinished(bool v)
{
	mIsFinished = v;
}

bool ServerApplication::isFinished()
{
	return mIsFinished;
}

bool ServerApplication::isStarted()
{
    return mIsStarted;
}

void ServerApplication::setStarted( bool v )
{
    mIsStarted = v;
}

void ServerApplication::start()
{
    run();
}

// 
// void libzfpt::ServerApplication::readProperty( jsoncpp::Value& _property )
// {
//     ScopedLock<CMutex> lock(m_propertyLock);
//     _property = m_property;
// }

string ServerApplication::getServerName()
{
    return mServerName;
}

void ServerApplication::setServerName( const string& name )
{
    mServerName=name;
}

ServerMetadata::~ServerMetadata()
{}

ServerMetadata::ServerMetadata()
{}

string ServerMetadata::toString()
{
    ScopedLock<Mutex> lock(mLock);
    //return mIntalVal.toString();
    return "";
}

void ServerMetadata::removeSection( const string& section )
{
    ScopedLock<Mutex> lock(mLock);
}

void ServerMetadata::removeKey( const string& section, const string& key )
{
    ScopedLock<Mutex> lock(mLock);
}

void ServerMetadata::setKeyValue( const string& section, const string& key, double val )
{
    ScopedLock<Mutex> lock(mLock);
    mIntalVal[section][key] = val;
}

void ServerMetadata::setKeyValue( const string& section, const string& key, int val )
{
    ScopedLock<Mutex> lock(mLock);
    mIntalVal[section][key] = val;
}

void ServerMetadata::setKeyValue( const string& section, const string& key, string val )
{
    ScopedLock<Mutex> lock(mLock);
    mIntalVal[section][key] = val;
}

bool ServerMetadata::isKeyExists( string section, string key )
{
    ScopedLock<Mutex> lock(mLock);
    return false;
}

bool ServerMetadata::isSectionExists( string section )
{
    ScopedLock<Mutex> lock(mLock);
    return false;
}

bool ServerMetadata::readAsBool( string section, string key, bool def/*=false*/ )
{
    ScopedLock<Mutex> lock(mLock);

    //mIntalVal[section][key];
    return false;
}

int ServerMetadata::readAsInt( string section, string key, int def/*=0*/ )
{
    ScopedLock<Mutex> lock(mLock);
//    return mCfg.readInt(section, key, def);
	return 0;
}

double ServerMetadata::readAsDouble( string section, string key, double def/*=0.0*/ )
{
    ScopedLock<Mutex> lock(mLock);
    //return mCfg.readDouble(section, key, def);
    return 0;
}

std::string ServerMetadata::readAsString( string section, string key, string def/*=""*/ )
{
    ScopedLock<Mutex> lock(mLock);
    //return mCfg.readString(section, key, def);
    return "";
}

