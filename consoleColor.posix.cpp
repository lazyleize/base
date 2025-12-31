/*
* @file    consoleColor.cpp
* @brief   终端颜色显示
* @author  leize<leize@xgd.com>
*
*/
#include <base/consoleColor.hpp>
#include <base/toolkit.hpp>

#include <cstdio>
#include <cstdlib>

#ifndef Z_OS_WIN32

using namespace aps;

using namespace std;

struct tagColorPair
{
    char front;
    char back;
};

bool ConsoleColor::sg_noColor = false;
void ConsoleColor::setColorMode(bool vEnableColor)
{
    sg_noColor = vEnableColor;
}


static stack<struct tagColorPair> colorStack[3];

//echo -e "\033[31;40;5m Shocking \033[0m"  
// \033[字符色;背景色;控制m ... \033[
ConsoleColor::ConsoleColor(eConsoleType ct, eConsoleColor _frontColor, eConsoleColor _backgroundColor)
:mConsoleType(ct)
{
    if (!sg_noColor)
        return ;
        
    bool hilight = false;
    if (_frontColor>=8)
        hilight = true;

    eConsoleColor frontColor = (eConsoleColor) (_frontColor % 8 + 30);
    eConsoleColor backgroundColor = (eConsoleColor) (_backgroundColor % 8 + 40);

    if ( (eConsoleTypeStdOut & mConsoleType) == eConsoleTypeStdOut )
    {
        // 判断上次颜色
        if (colorStack[eConsoleTypeStdOut].size()!=0)
        {
            fprintf(stdout, "\033[0m");
            fflush(stdout);
            //printf("~[0]");
        }

        // 插入当前颜色
        if (hilight)
        {
            fprintf(stdout, "\033[0m\033[%d;%d;1m", frontColor, backgroundColor);
            fflush(stdout);
        }
        else
        {
            fprintf(stdout, "\033[0m\033[%d;%dm", frontColor, backgroundColor);
            fflush(stdout);
        }

        //printf("[%d,%d]", frontColor, backgroundColor);

        tagColorPair colorPair;
        colorPair.back = _backgroundColor;
        colorPair.front = _frontColor;
        colorStack[eConsoleTypeStdOut].push( colorPair );
    }

    if ( (eConsoleTypeStdErr & mConsoleType) == eConsoleTypeStdErr )
    {
        // 判断上次颜色
        if (colorStack[eConsoleTypeStdErr].size()!=0)
        {
            fprintf(stderr, "\033[0m");
            fflush(stderr);
            //printf("~[0]");
        }

        // 插入当前颜色
        if (hilight)
        {
            fprintf(stderr, "\033[0m\033[%d;%d;1m", frontColor, backgroundColor);
            fflush(stderr);
        }
        else
        {
            fprintf(stderr, "\033[0m\033[%d;%dm", frontColor, backgroundColor);
            fflush(stderr);
        }

        //printf("[%d,%d]", frontColor, backgroundColor);

        tagColorPair colorPair;
        colorPair.back = _backgroundColor;
        colorPair.front = _frontColor;
        colorStack[eConsoleTypeStdErr].push( colorPair );
    }

}

ConsoleColor::~ConsoleColor()
{
    if (!sg_noColor)
        return ;
    if ( (eConsoleTypeStdOut & mConsoleType) == eConsoleTypeStdOut )
    {
        // 弹出当前
        tagColorPair colorPairLast ;
		colorPairLast = colorStack[eConsoleTypeStdOut].top();

        fprintf(stdout, "\033[0m");
        fflush(stdout);

        colorStack[eConsoleTypeStdOut].pop();

        // 判断上次颜色
        if (colorStack[eConsoleTypeStdOut].size()!=0)
        {
            tagColorPair colorPairLast = colorStack[eConsoleTypeStdOut].top();
            eConsoleColor frontColor2 = (eConsoleColor) (colorPairLast.front % 8 + 30);
            eConsoleColor backgroundColor2 = (eConsoleColor) (colorPairLast.back % 8 + 40);
            // 恢复上次颜色
            if (colorPairLast.front>=8)
            {
                fprintf(stdout, "\033[0m\033[%d;%d;1m", frontColor2, backgroundColor2);
                fflush(stdout);
            }
            else
            {
                fprintf(stdout, "\033[0m\033[%d;%dm", frontColor2, backgroundColor2);
                fflush(stdout);
            }

            //printf("[%d,%d]", frontColor2, backgroundColor2);
        }
    }

    if ( (eConsoleTypeStdErr & mConsoleType) == eConsoleTypeStdErr )
    {
        // 弹出当前
        tagColorPair colorPairLast = colorStack[eConsoleTypeStdErr].top();
        fprintf(stderr, "\033[0m");
        fflush(stderr);
        //printf("~[0]");
        colorStack[eConsoleTypeStdErr].pop();

        // 判断上次颜色
        if (colorStack[eConsoleTypeStdErr].size()!=0)
        {
            colorPairLast = colorStack[eConsoleTypeStdErr].top();
            eConsoleColor frontColor2 = (eConsoleColor) (colorPairLast.front % 8 + 30);
            eConsoleColor backgroundColor2 = (eConsoleColor) (colorPairLast.back % 8 + 40);
            // 恢复上次颜色
            if (colorPairLast.front>=8)
            {
                fprintf(stderr, "\033[0m\033[%d;%d;1m", frontColor2, backgroundColor2);
                fflush(stderr);
            }
            else
            {
                fprintf(stderr, "\033[0m\033[%d;%dm", frontColor2, backgroundColor2);
                fflush(stderr);
            }
            //printf("[%d,%d]", frontColor2, backgroundColor2);
        }
    }
}

#endif
