/*
* @file        unitTestBase.hpp
* @brief       单元测试工具类
* @author      leize<leize@xgd.com>
*
* @copyright   xgd
*/
#ifndef __LIBAPS_UNITBASE_HPP__
#define __LIBAPS_UNITBASE_HPP__

#include <base/platform.hpp>
#include <base/consoleApplication.hpp>
#include <string>

namespace aps
{
	using namespace std;
	using namespace aps;

	//! 单元测试类
	class LIBAPS_API UnittestBase:public ConsoleApplication
	{
	public:
		UnittestBase(int argc,char** argv);
		virtual ~UnittestBase();
		int run();

		void testValue(int Line,const char* File,const char*CodeString, bool Val);
		virtual void doTest()=0;

	protected:
		int m_passed;
		int m_failed;
		int m_total;
		string mModName;
	};

#define Z_UNITTEST_ASSERT_TRUE(cond) {m_total++;testValue(__LINE__, __FILE__, #cond, (cond) );}
}


#endif // !__LIBAPS_UNITBASE_HPP__