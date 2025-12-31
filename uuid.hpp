/*
* @file        uuid.hpp
* @brief       UUID
*   摘自: http://k3d-bart.googlecode.com/hg//k3dsdk/CUUID.h
* @author      UUID
*
* @copyright   Copyright (c) 2021 xgd
*/
#ifndef __LIBAPS_UUID_HPP__
#define __LIBAPS_UUID_HPP__

#include <string>
#include <base/platform.hpp>

namespace aps 
{
    using namespace std;

    //! UUID类
    class LIBAPS_API Uuid
    {
    public:
        Uuid();
        Uuid(const unsigned int Data1, const unsigned int Data2, const unsigned int Data3, const unsigned int Data4);

        static Uuid null();
        static Uuid getUUID();
        
        string toString();

        unsigned int data1;
        unsigned int data2;
        unsigned int data3;
        unsigned int data4;

        friend bool operator<(const Uuid& LHS, const Uuid& RHS);
        friend bool operator==(const Uuid& LHS, const Uuid& RHS);
        friend bool operator!=(const Uuid& LHS, const Uuid& RHS);
    };

    bool operator<(const Uuid& LHS, const Uuid& RHS);
    bool operator==(const Uuid& LHS, const Uuid& RHS);
    bool operator!=(const Uuid& LHS, const Uuid& RHS);
}

#endif // !__LIBAPS_UUID_HPP__
