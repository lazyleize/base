/*
* @file        datetime.hpp
* @brief       日期、日期函数
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#ifndef __LIBAPS_DATETIME_HPP__
#define __LIBAPS_DATETIME_HPP__

#include <base/config_.hpp>

#include <string>
#include <time.h>
#include <ctime>
#ifndef Z_OS_WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

namespace aps
{
	using namespace std;

	//! 日期类 
	class LIBAPS_API Date
	{
	public:

		Date();
		Date(struct  tm*tm_);
		Date(const Date& day);
		Date(int year,int month,int day);

		//成员读取
		int getYear()const;
		int getMonth()const;
		int getDay()const;
		bool setYear(int year);
		bool setMonth(int month);
		bool setDay(int day);

		// 日期增减
		Date& addDays(int day);
		Date& addMonths(int day);
		Date& addYears(int day);

		bool setDate(int year,int month,int day);
		bool setDate(string dateString);

		/// 日期比较 
        int compare(const Date& day) const;
        bool operator<(const Date& day) const { return compare(day) <  0; }
        bool operator<=(const Date& day) const { return compare(day) <= 0; }
        bool operator>(const Date& day) const { return compare(day) >  0; }
        bool operator>=(const Date& day) const { return compare(day) >= 0; }
        bool operator==(const Date& day) const { return compare(day) == 0; }
        bool operator!=(const Date& day) const { return compare(day) != 0; }
        Date& operator=(const Date& day);
        Date& operator++();
        Date operator++(int); // Date++;
        Date& operator--();
        Date operator--(int);
        Date& operator+=(int days);
        Date& operator-=(int days);

        friend Date operator+(const Date& day, int days);
        friend Date operator+(int days, const Date& day);
        friend Date operator-(const Date& day, int days);
        friend int operator-(const Date& day, const Date& twoDay);

        string toString() const;
        const char* toCStr() const;
        int toDays() const;

        /// @brief 格式化日期
        ///
        /// y: 两位年份 Y: 四位年份 m:1位月份 M:两位年份 d:无前缀日 D:两位日
        /// y: 11 Y: 2011 m:1 M:01 d:1 D:01 
        string format(string fmtStr) const;
        bool isNull() const {return mJulianDays==0;};
        void setNull() {mJulianDays=0;};

        // 一些工具函数
        int isLeap() const;
        int getDayOfWeek() const;
        int getDayOfYear() const;
        int getWeekOfYear() const;
        int getDaysOfMonth(unsigned int) const;

        static bool isValid(int year, int month, int day);
        static int getDaysOfMonth(unsigned int year, unsigned int month);
        static int isLeap(int year);
        static Date now();
    private:
        static int mDateTab[2][13];
        int mJulianDays;
        mutable char mBuffer[20];
	};

	//! 时间类
	class LIBAPS_API Time
	{
	public:
		Time();
		Time(tm* time);
		Time(const Time& time_);
		Time(int hour, int minute, int second, int millSend);

		int getHour()const;
		int getMinute()const;
		int getSecond()const;
		int getMillSecond()const;

		Time& setHour(int v);
		Time& setMinute(int v);
		Time& setSecond(int v);
		bool setTime(int hour,int minute,int second,int millSecond);
		bool setTime(string dateString);

		Time& addHours(int hours);
		Time& addMinutes(int minutes);
		Time& addSeconds(int seconds);

		int  compare(const Time& time_) const;
        bool operator<(const Time& time_) const { return compare(time_) <  0; }
        bool operator<=(const Time& time_) const { return compare(time_) <= 0; }
        bool operator>(const Time& time_) const { return compare(time_) >  0; }
        bool operator>=(const Time& time_) const { return compare(time_) >= 0; }
        bool operator==(const Time& time_) const { return compare(time_) == 0; }
        bool operator!=(const Time& time_) const { return compare(time_) != 0; }
        Time& operator=(const Time& time_);
        Time& operator+=(int seconds);
        Time& operator-=(int seconds);
        Time& operator++();
        Time operator++ (int);
        Time& operator--();
        Time operator--(int);
        friend Time operator+(const Time& time_, int seconds);
        friend Time operator+(int seconds, const Time& time_);
        friend Time operator-(const Time& time_, int seconds);
        friend int operator-(const Time& time_, const Time& twoTime);

        const char* toCStr() const;
        string toString() const;
        string  format(string) const;
        TInt64  toSeconds() const;
        static TInt64 toSeconds(const Time& time_);
        static Time now();
        
        static bool isValid(int hour, int minute, int second, int millSecond);
        bool isNull() const;
        void setNull();

	private:
		TInt64 mMillSeconds;
		mutable char mBuffer[27];
	};

	//! 时间日期类
	class LIBAPS_API Datetime
	{
	public:
		Datetime();
		Datetime(tm*);
		Datetime(TInt64 timpstamp);
		Datetime(int year,int month,int day);
		Datetime(const Date&date_);
		Datetime(int year,int month,int day,int hour,int minute,int second,int millSecond);
		Datetime(const Date& date_,const Time&tm_);
		Datetime(const Datetime& other);

		static Datetime now();

		Datetime& addHours(int hours);
		Datetime& addMinutes(int minutes);
        Datetime& addSeconds(int seconds);
        Datetime& addDays(int day);
        Datetime& addMonths(int month);
        Datetime& addYears(int year);

		int getDay()  const { return m_date.getDay();    }
        int getMonth() const { return m_date.getMonth();  }
        int getYear() const { return m_date.getYear();   }
        int getHour() const { return m_time.getHour();   }
        int getMinute() const { return m_time.getMinute(); }
        int getSecond() const { return m_time.getSecond(); }
        int getMillSecond() const { return m_time.getMillSecond();}
        Date getDate() const { return m_date; }
        Time getTime() const { return m_time; }
        static TInt64  toUnixTimestamp(const Datetime& tm_);
        static TInt64 getCurUnixTimestamp();
        TInt64  toUnixTimestamp() const;
        bool setDatetime(int year, int month, int day, int hour, int minute, int second, int millSecond);
        bool setDatetime(string);

        const char* toCStr();
        string toString() const;
        string format(string) const;

        int compare(const Datetime& time_) const;
        bool operator<(const Datetime& time_) const{ return compare(time_) <  0; }
        bool operator<=(const Datetime& time_) const{ return compare(time_) <= 0; }
        bool operator>(const Datetime& time_) const{ return compare(time_) >  0; }
        bool operator>=(const Datetime& time_) const{ return compare(time_) >= 0; }
        bool operator==(const Datetime& time_) const{ return compare(time_) == 0; }
        bool operator!=(const Datetime& time_) const{ return compare(time_) != 0; }
        Datetime& operator=(const Datetime& time_);
        Datetime& operator+=(TInt64 seconds);
        Datetime& operator-=(TInt64 seconds);
        Datetime& operator++();
        Datetime operator++ (int);
        Datetime& operator--();
        Datetime operator--(int);
        friend Datetime  LIBAPS_API operator+(const Datetime& time_, TInt64 seconds);
        friend Datetime  LIBAPS_API operator+(TInt64 seconds, const Datetime& time_);
        friend Datetime  LIBAPS_API operator-(const Datetime& time_, TInt64 seconds);
        friend TInt64 LIBAPS_API operator-(const Datetime& time_, const Datetime& twoTime);
        
        Date& date();
        Time& time();

        bool isNull() const {return m_date.isNull() && m_time.isNull();};
        void setNull() {m_date.setNull();m_time.setNull();};

	private:
		Date m_date;
		Time m_time;
		char mBuffer[40];
	};
}
#endif //!__LIBAPS_DATETIME_HPP__