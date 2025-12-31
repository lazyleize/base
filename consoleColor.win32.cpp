/*
* @file    consoleColor.cpp
* @brief   终端颜色显示
* @author  leize<leize@xgd.com>
*
*/
#include <base/consoleColor.hpp>
#include <stdio.h>
#include <stdlib.h>

#ifdef Z_OS_WIN32

using namespace aps;
using namespace std;

bool ConsoleColor::sg_noColor = true;
void ConsoleColor::setColorMode(bool vEnableColor)
{
    sg_noColor = vEnableColor;
}

static void setColorAndSaveOld(DWORD hdtype, eConsoleColor f, eConsoleColor b, eConsoleColor& oldf, eConsoleColor& oldb)
{
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(hdtype), &csbiInfo)) 
	{
		oldf = eConColorWhite;
		oldb = eConColorBlack;
	}
	else
	{
		oldf = (eConsoleColor) (csbiInfo.wAttributes % 16);
        oldb = (eConsoleColor) (csbiInfo.wAttributes / 16);
	}
	SetConsoleTextAttribute(GetStdHandle(hdtype), b*16+f);
}

ConsoleColor::ConsoleColor(eConsoleType ct, eConsoleColor frontColor, eConsoleColor backgroundColor)
:mConsoleType(ct)
{
	if((eConsoleTypeStdOut & mConsoleType) == eConsoleTypeStdOut)
		setColorAndSaveOld(STD_OUTPUT_HANDLE,frontColor,backgroundColor,mOldFrontColor[eConsoleTypeStdOut],mOldBackgroundColor[eConsoleTypeStdOut]);
	if ( (eConsoleTypeStdErr & mConsoleType) == eConsoleTypeStdErr )
        setColorAndSaveOld(STD_ERROR_HANDLE, frontColor, backgroundColor, mOldFrontColor[eConsoleTypeStdErr],mOldBackgroundColor[eConsoleTypeStdErr] );
}

ConsoleColor::~ConsoleColor()
{
    if ( (eConsoleTypeStdOut & mConsoleType) == eConsoleTypeStdOut )
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), mOldBackgroundColor[eConsoleTypeStdOut]*16+mOldFrontColor[eConsoleTypeStdOut]);
    if ( (eConsoleTypeStdErr & mConsoleType) == eConsoleTypeStdErr )
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), mOldBackgroundColor[eConsoleTypeStdErr]*16+mOldFrontColor[eConsoleTypeStdErr]);
}

#endif