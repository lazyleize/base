/*
* @file    utility.cpp
* @brief   单元测试工具类
* @author  leize<leize@xgd.com>
*
*/
#include <base/unittest.hpp>
#include <base/toolkit.hpp>
#include <base/consoleColor.hpp>
#include <base/datetime.hpp>
#include <base/exception.hpp>

#include <stdio.h>
#include <stdlib.h>
#include "base/strHelper.hpp"

using namespace aps;
using namespace std;

#define NOW_DATETIME_STR_ Datetime::now().toCStr()

UnittestBase::UnittestBase(int argc, char**argv)
:ConsoleApplication(argc, argv)
{
    m_passed = 0;
    m_failed= 0;
    m_total= 0;
}

UnittestBase::~UnittestBase()
{
}

void UnittestBase::testValue(int Line, const char*File, const char*CodeString, bool Val)
{
    string strCode = StrHelper::replace(StrHelper::trim(CodeString), "\n", "");
    strCode = StrHelper::replace(strCode, "\r", "");

    printf(_G2A("第% 3d项测试 ["), m_total);
    if (Val)
    {
        {
            ConsoleColor co(eConsoleTypeStdOut, eConColorLightGreen);
            printf("%s", _G2A("通过"));
        }
        printf("] ");

        ConsoleColor co(eConsoleTypeStdOut, eConColorGray);
        printf("%s\n", strCode.c_str());

        m_passed++;
    }
    else
    {
        {
            ConsoleColor co(eConsoleTypeStdOut, eConColorLightRed);
            printf("%s", _G2A("失败"));
        }
        printf(_G2A("]\n  条件: %s\n  代码: %s, 第%d行.\n"), strCode.c_str(), File, Line);

        m_failed++;
    }
}


int UnittestBase::run()
{
    {
        ConsoleColor co(eConsoleTypeStdOut, eConColorGreen);
        printf(_G2A("[%s] 模块 %s 开始\n"), NOW_DATETIME_STR_, mModName.c_str());
    }

    Exception _error;
    int r = -1;
    try
    {
        doTest();
        r = 0;
    }
    catch (Exception& e)
    {  
        _error = e;
    }
    catch (...)
    {
        _error = Exception(Z_SOURCEINFO, -1, "Unknown exception");
    }

    {
        ConsoleColor co(eConsoleTypeStdOut, eConColorYellow);

        double rate = 0;
        if (m_total!=0)
        {
            rate = 100.00*m_passed/m_total;
        }

        printf(_G2A("[%s] 模块 %s 结束, %%%0.2lf通过, 其中"), NOW_DATETIME_STR_, mModName.c_str(), rate);

        {
            ConsoleColor co(eConsoleTypeStdOut, eConColorLightRed);
            printf("% 2d", m_failed);
        }

        printf("%s", _G2A("项失败.\n\n"));
    }

    if (r==-1)
        throw Exception(_error);

    return 0;
}

#undef NOW_DATETIME_STR_