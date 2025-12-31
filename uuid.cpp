/*
* @file        uuid.cpp
* @brief       UUID
*   摘自: http://k3d-bart.googlecode.com/hg//k3dsdk/CUUID.h
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/

#include <base/uuid.hpp>

#include <cstring>
#include <cstdio>
#include <ctime>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <iomanip>
#endif 

using namespace std;

aps::Uuid::Uuid():
data1(0),
data2(0),
data3(0),
data4(0)
{
}

aps::Uuid::Uuid(const unsigned int Data1, const unsigned int Data2, const unsigned int Data3, const unsigned int Data4) :
data1(Data1),
data2(Data2),
data3(Data3),
data4(Data4)
{
}

aps::Uuid aps::Uuid::null()
{
    return aps::Uuid(0, 0, 0, 0);
}

static double random(double start, double end)
{
    static bool bInit = false;
    if (!bInit)
    {
        bInit = true;
        srand(unsigned(time(0)));
    }
    double v= start+(end-start)*rand()/(RAND_MAX + 1.0);
    return v;
}

aps::Uuid aps::Uuid::getUUID()
{
    aps::Uuid result;

#ifdef _WIN32
    CoCreateGuid(reinterpret_cast<GUID*>(&result));
#else
#ifndef RAND_MAX
#define RAND_MAX 32767
#endif
    result.data1 = (int)random(1, RAND_MAX);
    result.data2 = (int)random(1, RAND_MAX);
    result.data3 = (int)random(1, RAND_MAX);
    result.data4 = (int)random(1, RAND_MAX);
#endif // !_WIN32
    
    return result;
}

bool operator<(const aps::Uuid& LHS, const aps::Uuid& RHS)
{
    if(LHS.data1 == RHS.data1)
    {
        if(LHS.data2 == RHS.data2)
        {
            if(LHS.data3 == RHS.data3)
            {
                return LHS.data4 < RHS.data4;
            }
            else
                return LHS.data3 < RHS.data3;
        }
        else
            return LHS.data2 < RHS.data2;
    }

    return LHS.data1 < RHS.data1;
}

bool aps::operator==(const aps::Uuid& LHS, const aps::Uuid& RHS)
{
    return (LHS.data1 == RHS.data1) && (LHS.data2 == RHS.data2) && (LHS.data3 == RHS.data3) && (LHS.data4 == RHS.data4);
}

bool aps::operator!=(const aps::Uuid& LHS, const aps::Uuid& RHS)
{
    return !(LHS == RHS);
}

string aps::Uuid::toString()
{
    string uuidstr = "{";
    char szBuffer[10]={0};

    memset(szBuffer, 0x00, sizeof(szBuffer));
    sprintf(szBuffer,"%x-", data1);
    uuidstr += szBuffer;

    memset(szBuffer, 0x00, sizeof(szBuffer));
    sprintf(szBuffer,"%x", data2);
    uuidstr += string(szBuffer).substr(0,4) + "-" + string(szBuffer).substr(4,4);

    memset(szBuffer, 0x00, sizeof(szBuffer));
    sprintf(szBuffer,"%x", data3);
    uuidstr += "-"+ string(szBuffer).substr(0,4) + "-" + string(szBuffer).substr(4,4);

    memset(szBuffer, 0x00, sizeof(szBuffer));
    sprintf(szBuffer,"%x", data4);
    uuidstr += string(szBuffer);
    
    uuidstr += "}";
    return uuidstr;
}
