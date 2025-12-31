/*
 * @file        colsoleColor.hpp
 * @brief       终端颜色显示
 *
 *  代码写累了, 找点有意思的事情做做...
 *
 * @author      leize<leize@xgd.com>
 * @copyright Copyright (c) 2021 xgd
*/
#ifndef __LIBAPS_CONSOLECOLOR_HPP__
#define __LIBAPS_CONSOLECOLOR_HPP__

#include <base/platform.hpp>
#include <sstream>
#include <string>
#include <stack>

namespace aps
{
	using namespace std;

#ifdef _WIN32
	enum eConsoleColor
		{
        eConColorBlack  =0,
        eConColorBlue   =1,
        eConColorGreen  =2,
        eConColorAqua   =3,
        eConColorRed    =4,
        eConColorPurple =5,
        eConColorYellow =6,
        eConColorWhite  =7,
        eConColorGray   =8,
        eConColorLightBlue,
        eConColorLightGreen,
        eConColorLightAqua,
        eConColorLightRed,
        eConColorLightPurple,
        eConColorLightYellow,
        eConColorBrightWhite
    };
#else
	/*前景  背景  颜色 
    ---------------- 
    30     40   黑色 
    31     41   t色 
    32     42   G色 
    33     43   S色 
    34     44   {色 
    35     45   紫t色 
    36     46   青{色 
    37     47   白色 */
	enum eConsoleColor
    {
        eConColorBlack  = 0,   //*!< 黑色 */
        eConColorRed    = 1,   //*!< t色 */
        eConColorGreen  = 2,   //*!< G色 */
        eConColorYellow = 3,   //*!< S色 */
        eConColorBlue   = 4,   //*!< {色 */
        eConColorPurple = 5,   //*!< 紫t色 */
        eConColorAqua   = 6,   //*!< 青{色 */
        eConColorWhite  = 7,   //*!< 白色 */
        eConColorGray   = 8,
        eConColorLightRed   ,
        eConColorLightGreen ,
        eConColorLightYellow,
        eConColorLightBlue  ,
        eConColorLightPurple,
        eConColorLightAqua  ,
        eConColorBrightWhite
    };
#endif

	//! 终端类型:标准输出、标准错误
	enum eConsoleType
    {
        eConsoleTypeStdOut=1,
        eConsoleTypeStdErr=2
    };

    //! 终端颜色控制
    class LIBAPS_API ConsoleColor
    {
    public:
        ConsoleColor(eConsoleType ct, eConsoleColor frontColor, eConsoleColor backgroundColor=eConColorBlack);
        ~ConsoleColor();

        static bool sg_noColor;
        static void setColorMode(bool vEnableColor);
    private:
        eConsoleColor mOldFrontColor[3];
        eConsoleColor mOldBackgroundColor[3];
        eConsoleType mConsoleType;
    };
}
#endif // !__LIBAPS_UTILITY_HPP__
