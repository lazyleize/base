/*
* @file        datetime.hpp
* @brief       日期、日期函数
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/

#include <base/datetime.hpp>
#include <base/exception.hpp>

#include <time.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifndef Z_OS_WIN32
#include <errno.h>
#endif

using namespace std;
using namespace aps;

namespace
{
	struct YMD_t
    {
        int year;   // [1900..2500]
        int month;  // [1..12]
        int day;    // [1..31]
    };
}

static YMD_t julianDaysToYmd(int julianDays);
static int  ymdToJulianDays(int year, int month, int day);

int Date::mDateTab[2][13] = 
{
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, 
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

static void formtDateTime(string &dateString, string fmtStr, int y_, int m_, int d_, int h_, int i_,int s_, int u_) 
{
	char szBuffer[20];
	char szFmtBuffer[20];

	enum eState
	{
		eStateOut,
		eStateIn
	};

	int state = eStateOut;
	char ch0;
	char ch1;
	for(size_t i=0;i<fmtStr.length();i++)
	{
		ch0 = fmtStr[i];
		switch(state)
		{
		case eStateOut:
			{
				if(ch0=='Y'||ch0=='M'||ch0=='D'||ch0=='y'||ch0=='m'||ch0=='d' || 
                    ch0=='H'||ch0=='I'||ch0=='S'||ch0=='h'||ch0=='i'||ch0=='s' || 
                    ch0=='U'||ch0=='u'
                    )
				{
					szFmtBuffer[0] = 0;

					//格式
					switch(ch0)
					{
					case 'Y':
						sprintf(szFmtBuffer,"%%04d");
						break;
					case 'M':
					case 'D':
					case 'y':
					case 'H':
					case 'I':
					case 'S':
						sprintf(szFmtBuffer,"%%02d");
						break;
					case 'U':
						sprintf(szFmtBuffer,"%%05d");
						break;
					default:
						sprintf(szFmtBuffer,"%%d");
					}

					//值
					int Val = 0;
					switch(ch0)
					{
					case 'Y':
						Val = y_;
						break;
					case 'y':
						Val = y_ % 100;
						break;
					case 'M':
					case 'm':
						Val = m_;
                        break;
					case 'D':
                    case 'd':
                        Val = d_;
                        break;
                    case 'H':
                    case 'h':
                        Val = h_;
                        break;
                    case 'I':
                    case 'i':
                        Val = i_;
                        break;
                    case 'S':
                    case 's':
                        Val = s_;
                        break;
                    case 'U':
                    case 'u':
                        Val = u_;
                        break;
					}

					//if (Val>=0)
                    {
                        snprintf(szBuffer, sizeof(szBuffer), szFmtBuffer, Val);
                        dateString += szBuffer;
                    }
					continue;
				}
				else if (fmtStr[i]=='\\')
				{
					if (i<fmtStr.length()-1)
                    {
                        ch1 = fmtStr[i+1];
                        if (ch1=='Y'||ch1=='M'||ch1=='D'||ch1=='y'||ch1=='m'||ch1=='d' || 
                            ch1=='H'||ch1=='I'||ch1=='S'||ch1=='h'||ch1=='i'||ch1=='s' || 
                            ch1=='U'||ch1=='u')
                        {
                            // 当前字符不能要
                            state = eStateIn;
                            continue;
                        }
                        else if (ch1=='\\')
                        {
                            // 当前字符不能要
                            state = eStateIn;
                            continue;
                        }
                    }
				}
			}
			break;
		case eStateIn:
            state = eStateOut;
            break;
        default:
            break;
		}

		dateString += ch0;
	}
}

Date::Date()
{
	mJulianDays = 0;

	time_t tval = time(0);
	struct tm* time_ = localtime(&tval);
	mJulianDays = ymdToJulianDays(time_->tm_year + 1900, time_->tm_mon + 1, time_->tm_mday);
}

Date::Date(int year,int month,int day)
{
	mJulianDays = 0;
	if(Date::isValid(year,month,day))
		mJulianDays = ymdToJulianDays(year,month,day);
}

Date::Date(const Date& oneDay):mJulianDays(oneDay.mJulianDays)
{
}

Date::Date(struct tm* time_)
{
	mJulianDays = ymdToJulianDays(time_->tm_year + 1900, time_->tm_mon + 1, time_->tm_mday);
}

Date& Date::operator=(const Date& oneDay)
{
	if(&oneDay != this)
		mJulianDays = oneDay.mJulianDays;
	return *this;
}

int Date::getDay()const
{
	if(isNull())
		return 1;
	return julianDaysToYmd(mJulianDays).day;
}

int Date::getMonth()const
{
	if(isNull())
		return 1;
	return julianDaysToYmd(mJulianDays).month;
}

int Date::getYear() const        
{ 
    if (isNull())
        return 1970;
    return julianDaysToYmd(mJulianDays).year;    
}

bool Date::setDate(int year, int month, int day)    
{ 
    if (month < 1 || month > 12 || day < 0 || day > mDateTab[isLeap(year)][month])
        return false;
    mJulianDays = ymdToJulianDays(year, month,day);
    return true;
}

bool Date::setYear(int year)    
{ 
    if (year<1900||year>2500)
        return false;
    mJulianDays = ymdToJulianDays(year, getMonth(), getDay());
    return true;
}

bool Date::setMonth(int month)    
{ 
    if (month < 1 || month > 12)
        return false;
    mJulianDays = ymdToJulianDays(getYear(), month, getDay());
    return true;
}

bool Date::setDay(int day)    
{ 
    if (day < 0 || day > mDateTab[isLeap(getYear())][getMonth()])
        return false;
    mJulianDays = ymdToJulianDays(getYear(), getMonth(),day);
    return true;
}

int Date::isLeap(int year)
{
	return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int Date::isLeap()const
{
	return Date::isLeap(getYear());
}

int Date::getDayOfWeek()const
{
	return (mJulianDays+1) % 7;
}

int Date::getDayOfYear()const
{
	YMD_t ymd(julianDaysToYmd(mJulianDays));
    int bIsLeap = isLeap();
    int days = 0;
    days += ymd.day;
    for (int i=1;i<ymd.month;i++)
    {
        days += mDateTab[bIsLeap][i];
    }
    return days;
}

static const int _getWeekOfYear(const int year, const int month, const int day)
{
    // Algorithm from http://personal.ecu.edu/mccartyr/ISOwdALG.txt

    const bool leapYear = ((year % 4) == 0 && (year % 100) != 0) || (year % 400) == 0;
    const bool leapYear_1 = (((year - 1) % 4) == 0 && ((year - 1) % 100) != 0) || ((year - 1) % 400) == 0;

    // 4. Find the DayOfYearNumber for Y M D
    static const int DAY_OF_YEAR_NUMBER_MAP[12] =
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

    int DayOfYearNumber = day + DAY_OF_YEAR_NUMBER_MAP[month - 1];

    if (leapYear && month > 2)
        DayOfYearNumber += 1;

    // 5. Find the Jan1Weekday for Y (Monday=1, Sunday=7)
    const int YY = (year - 1) % 100;
    const int C = (year - 1) - YY;
    const int G = YY + YY / 4;
    const int Jan1Weekday = 1 + (((((C / 100) % 4) * 5) + G) % 7);

    // 6. Find the Weekday for Y M D
    const int H = DayOfYearNumber + (Jan1Weekday - 1);
    const int Weekday = 1 + ((H - 1) % 7);

    // 7. Find if Y M D falls in YearNumber Y-1, WeekNumber 52 or 53
    int YearNumber = 0, WeekNumber = 0;

    if (DayOfYearNumber <= (8 - Jan1Weekday) && Jan1Weekday > 4)
    {
        YearNumber = year - 1;

        if (Jan1Weekday == 5 || (Jan1Weekday == 6 && leapYear_1))
            WeekNumber = 53;
        else
            WeekNumber = 52;
    }
    else
    {
        YearNumber = year;
    }

    // 8. Find if Y M D falls in YearNumber Y+1, WeekNumber 1
    if (YearNumber == year)
    {
        const int I = (leapYear ? 366 : 365);

        if ((I - DayOfYearNumber) < (4 - Weekday))
        {
            YearNumber = year + 1;
            WeekNumber = 1;
        }
    }

    // 9. Find if Y M D falls in YearNumber Y, WeekNumber 1 through 53
    if (YearNumber == year)
    {
        const int J = DayOfYearNumber + (7 - Weekday) + (Jan1Weekday - 1);

        WeekNumber = J / 7;

        if (Jan1Weekday > 4)
            WeekNumber -= 1;
    }

    return WeekNumber;
}

int Date::getWeekOfYear() const
{
    YMD_t ymd(julianDaysToYmd(mJulianDays));
    return _getWeekOfYear(ymd.year, ymd.month, ymd.day);
}

int Date::getDaysOfMonth(unsigned int month)const
{
	return mDateTab[isLeap()][month % 13];
}

int Date::getDaysOfMonth(unsigned int year, unsigned int month)
{
    return mDateTab[Date::isLeap(year)][month % 13];
}

int Date::compare(const Date& oneDay) const
{
    return mJulianDays-oneDay.mJulianDays;
}

Date Date::now()
{
    time_t tval = time(0);
    struct tm *tmp= localtime(&tval);
    return Date(tmp);
}

string Date::toString() const
{
    return format("Y-M-D");
}

const char* Date::toCStr() const
{
    strcpy(mBuffer, toString().c_str());
    return mBuffer;
}

bool Date::setDate(string dateString)
{
    int nYear=0;
    int nMonth=0;
    int nDay=0;
    int nTmp=0;

    int match = sscanf(dateString.c_str(),"%d-%d-%d %d:%d:%d", &nYear,&nMonth,&nDay,&nTmp,&nTmp,&nTmp);

    if (match==6)
    {
        return setDate(nYear, nMonth, nDay);
    }

    match = sscanf(dateString.c_str(),"%d-%d-%d", &nYear,&nMonth,&nDay);

    if (match==3)
    {
        return setDate(nYear, nMonth, nDay);
    }

    match = sscanf(dateString.c_str(),"%d/%d/%d %d:%d:%d", &nYear,&nMonth,&nDay,&nTmp,&nTmp,&nTmp);
    if (match==6)
    {
        return setDate(nYear, nMonth, nDay);
    }

    match = sscanf(dateString.c_str(),"%d/%d/%d", &nYear,&nMonth,&nDay);
    if (match==3)
    {
        return setDate(nYear, nMonth, nDay);
    }

    match = sscanf(dateString.c_str(),"%04d%02d02%02d%02d%02d%02d", &nYear,&nMonth,&nDay,&nTmp,&nTmp,&nTmp);
    if (match==6)
    {
        return setDate(nYear, nMonth, nDay);
    }

    match = sscanf(dateString.c_str(),"%04d%02d%02d", &nYear,&nMonth,&nDay);

    if (match==3)
    {
        return setDate(nYear, nMonth, nDay);
    }

    return false;
}

/*
Y -> 2011 y -> 11
m -> 12
d -> 05
*/
string Date::format(string fmtStr) const
{
    string retString ;
    YMD_t ymd;
    ymd = julianDaysToYmd(mJulianDays);
    if (isNull())
        return "1970-01-01";

    formtDateTime(retString, fmtStr, ymd.year, ymd.month, ymd.day, -1, -1, -1, -1);
    return retString;
}

Date operator+(const Date& oneDay, int days)
{
    Date result(oneDay);
    return result += days;
}

Date operator+(int days, const Date& oneDay)
{
    Date result(oneDay);
    return result += days;
}

Date operator-(const Date& oneDay, int days)
{
    Date result(oneDay);
    return result -= days;
}

namespace aps
{
int operator-(const Date& oneDay, const Date& twoDay)
{
    return oneDay.mJulianDays - twoDay.mJulianDays;
}
}


Date& Date::operator+=(int days)
{
    mJulianDays += days;
    return *this;
}

Date& Date::operator-=(int days)
{
    return *this += (-days);
}

Date& Date::operator++()
{
    return ((*this) += 1);
}

Date Date::operator++(int)  // postfix
{
    Date retval = *this;
    (*this) += 1;
    return retval;
}

Date& Date::operator--()
{
    return ((*this) -= 1);
}

Date Date::operator--(int)  // postfix
{
    Date retval = *this;
    (*this) -= 1;
    return retval;
}

int Date::toDays() const
{
    return mJulianDays;
}

bool Date::isValid(int year, int month, int day)
{
    if ( month < 1 || month > 12 || day < 0 || day > mDateTab[isLeap(year)][month])
        return false;
    return true;
}

YMD_t julianDaysToYmd(int julianDays) 
{
    int a = julianDays + 32044;
    int b = (4 * a + 3) / 146097;
    int c = a - ((b * 146097) / 4);
    int d = (4 * c + 3) / 1461;
    int e = c - ((1461 * d) / 4);
    int m = (5 * e + 2) / 153;

    YMD_t ymd;
    ymd.day = e - ((153 * m + 2) / 5) + 1;
    ymd.month = m + 3 - 12 * (m / 10);
    ymd.year = b * 100 + d - 4800 + (m / 10);
    return ymd;
}

int ymdToJulianDays(int year, int month, int day)
{
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    return day + (153*m + 2) / 5 + y*365 + y/4 - y/100 + y/400 - 32045;
}   

Date& Date::addDays( int day )
{
    mJulianDays += day;
    return *this;
}

Date& Date::addMonths( int month )
{
    YMD_t ymd;
    ymd = julianDaysToYmd(mJulianDays);

    int years = month / 12;
    ymd.year += years;
    month = month % 12;

    if (month>0)
    {
        if (ymd.month+month>12)
        {
            ymd.year++;
            ymd.month = ymd.month+month-12;
        }
        else 
        {
            ymd.month = ymd.month+month;
        }
    }
    else if (month<0)
    {
        if (ymd.month+month<0)
        {
            ymd.year--;
            ymd.month = ymd.month+month+12;
        }
        else 
        {
            ymd.month = ymd.month+month;
        }
    }

    mJulianDays = ymdToJulianDays(ymd.year, ymd.month, ymd.day);
    return *this;
}

Date& Date::addYears( int year )
{
    YMD_t ymd;
    ymd = julianDaysToYmd(mJulianDays);
    ymd.year += year;

    mJulianDays = ymdToJulianDays(ymd.year, ymd.month, ymd.day);
    return *this;
}

//////////////////////////////////////////////////

Time::Time()
{
    mMillSeconds = 0;

    struct timeval tp;
    gettimeofday(&tp,NULL);

    struct tm *newtime;
    time_t tm_t = tp.tv_sec;
    newtime = localtime( &tm_t );
    mMillSeconds = (newtime->tm_hour * 3600 + newtime->tm_min*60 + newtime->tm_sec)*1000UL + tp.tv_usec/1000UL;
}

Time::Time(int hour, int minute, int second, int millSecond)
{
    setTime(hour, minute, second, millSecond);
}

Time::Time(tm* tm_)
{
    if (NULL!=tm_)
    {
        mMillSeconds = (tm_->tm_hour*3600+tm_->tm_min*60+tm_->tm_sec)*1000;
    }
}

Time::Time(const Time &tm_)
{
    mMillSeconds = tm_.mMillSeconds;
}

Time& Time::operator=(const Time& oneTime)
{
    if (&oneTime != this)
    {
        mMillSeconds = oneTime.mMillSeconds;
    }
    return *this; 
}

int Time::getHour() const
{
    return (int)( (mMillSeconds / (3600*1000UL)) % 24 );
}

int Time::getMinute() const
{
    return (int) ( mMillSeconds % (3600*1000UL) ) / (60*1000UL);
}

int Time::getSecond() const
{
    return (int)( (mMillSeconds % (60*1000UL)) / 1000L );
}

int Time::getMillSecond() const
{
    return (int)( mMillSeconds % 1000UL );
}

bool Time::setTime(int hour, int minute, int second, int millSecond)
{
    if (Time::isValid(hour, minute, second, millSecond))
    {
        mMillSeconds = (hour*3600+minute*60+second)*1000UL + millSecond;
        return true;
    }
    return false;
}

int Time::compare(const Time& oneTime) const
{
    return (int)(mMillSeconds - oneTime.mMillSeconds);
}

string Time::toString() const
{
    return format("H:I:S");
}

const char* Time::toCStr() const 
{
    strcpy(mBuffer, toString().c_str());
    return mBuffer;
}

bool Time::setTime(string dateString)
{
    int nHour=0;
    int nMinute=0;
    int nSecond=0;
    int nTmp=0;

    int match = sscanf(dateString.c_str(),"%d-%d-%d %d:%d:%d.%d", &nTmp,&nTmp,&nTmp,&nHour,&nMinute,&nSecond, &nTmp);
    if (match==7)
    {
        return setTime(nHour, nMinute, nSecond, nTmp/1000UL);
    }

    match = sscanf(dateString.c_str(),"%d/%d/%d %d:%d:%d.%d", &nTmp,&nTmp,&nTmp,&nHour,&nMinute,&nSecond, &nTmp);
    if (match==7)
    {
        return setTime(nHour, nMinute, nSecond, nTmp/1000UL);
    }

    match = sscanf(dateString.c_str(),"%d-%d-%d %d:%d:%d", &nTmp,&nTmp,&nTmp,&nHour,&nMinute,&nSecond);
    if (match==6)
    {
        return setTime(nHour, nMinute, nSecond, 0);
    }

    match = sscanf(dateString.c_str(),"%d:%d:%d", &nHour,&nMinute,&nSecond);
    if (match==3)
    {
        return setTime(nHour, nMinute, nSecond, 0);
    }

    match = sscanf(dateString.c_str(),"%d/%d/%d %d:%d:%d", &nTmp,&nTmp,&nTmp,&nHour,&nMinute,&nSecond);
    if (match==6)
    {
        return setTime(nHour, nMinute, nSecond, 0);
    }

    return false;
}

/*
* H I S.u
*/
string Time::format(string fmtStr) const
{
    string retString;

    if (isNull())
        return "00:00:00";

    formtDateTime(retString, fmtStr, -1, -1, -1, getHour(), getMinute(), getSecond(), getMillSecond());
    return retString;
}

TInt64 Time::toSeconds() const
{
    return mMillSeconds / 1000;
}

Time Time::now()
{
    struct timeval tp;
    gettimeofday(&tp,NULL);
    Time newTm;

    struct tm *newtime;
    time_t tm_t = tp.tv_sec;
    newtime = localtime( &tm_t );
    newTm.mMillSeconds = (newtime->tm_hour * 3600 + newtime->tm_min*60 + newtime->tm_sec)*1000UL + tp.tv_usec/1000UL;

    return newTm;
}

Time operator+(Time& oneTime, int seconds)
{
    Time result(oneTime);
    return result += seconds;
}

Time operator+(int seconds, const Time& oneTime)
{
    Time result(oneTime);
    return result += seconds;
}
namespace aps
{

Time operator-(const Time& oneTime, int seconds)
{
    Time result(oneTime);
    return result -= seconds;
}

int operator-(const Time& oneTime, const Time& twoTime)
{
    int v = (int)(oneTime.mMillSeconds-twoTime.mMillSeconds);
    if (v!=0)
    {
        if (v>0)
            return (int)(v/1000) + 1;
        else 
            return (int)(v/1000) - 1;
    }
    return 0;
}

Time& Time::operator+=(int seconds)
{
    mMillSeconds += seconds*1000L;
    return *this;
}

Time& Time::operator-=(int seconds)
{
    *this += (-1*seconds);
    return *this;
}

Time& Time::operator++()
{
    mMillSeconds += 1000L;
    return *this;
}

Time Time::operator++(int)
{
    Time tm_(*this);
    tm_.mMillSeconds += 1000L;
    return tm_;
}

Time& Time::operator--()
{
    mMillSeconds -= 1000L;
    return *this;
}

Time Time::operator--(int)
{
    Time tm_(*this);
    tm_.mMillSeconds -= 1000L;
    return tm_;
}
}

bool Time::isValid(int hour, int minute, int second, int millSecond)
{
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59 
        || millSecond < 0 || millSecond >= 1000L )
    {
        return false;
    }
    return true;
}

Time& Time::addHours( int hours )
{
    mMillSeconds += hours*3600*1000;
    mMillSeconds = mMillSeconds % (24*3600*1000);
    if (mMillSeconds<0)
        mMillSeconds += 24*3600*1000;
    return *this;
}

Time& Time::addMinutes( int minutes )
{
    mMillSeconds += minutes*60*1000;
    mMillSeconds = mMillSeconds % (24*3600*1000);
    if (mMillSeconds<0)
        mMillSeconds += 24*3600*1000;
    return *this;
}

Time& Time::addSeconds( int seconds )
{
    mMillSeconds += seconds*1000;
    mMillSeconds = mMillSeconds % (24*3600*1000);
    if (mMillSeconds<0)
        mMillSeconds += 24*3600*1000;
    return *this;
}

Time& Time::setHour( int v )
{
    setTime(v, getMinute(), getSecond(), getMillSecond());
    return *this;
}

Time& Time::setMinute( int v )
{
    setTime(getHour(), v, getSecond(), getMillSecond());
    return *this;
}

Time& Time::setSecond( int v )
{
    setTime(getHour(), getMinute(),v, getMillSecond());
    return *this;
}

bool Time::isNull() const
{
    return mMillSeconds==-1;
}

void Time::setNull()
{
    mMillSeconds=-1;
}

//////////////////////////

Datetime::Datetime()
{
}

Datetime::Datetime(tm*tm_)
:m_date(tm_), m_time(tm_)
{
}

Datetime::Datetime(int year, int month, int day)
:m_date(year, month, day)
{
}

Datetime::Datetime(const Date&date_)
:m_date(date_)
{
}

Datetime::Datetime(TInt64 timpstamp)
{
    timpstamp += 8*3600UL;
    *this = Datetime(1970,1,1,0,0,0,0) + timpstamp;
}

Datetime::Datetime(int year, int month, int day, int hour, int minute, int second, int millSecond)
:m_date(year,month,day),
m_time(hour, minute, second, millSecond)
{
}

Datetime::Datetime(const Date&date_, const Time&tm_)
:m_date(date_), m_time(tm_)
{
}

Datetime::Datetime(const Datetime& other)
{
    m_time = other.m_time;
    m_date = other.m_date;
}

Datetime Datetime::now()
{
    struct timeval tp;
    gettimeofday(&tp,NULL);
    Time newTm;

    struct tm *newtime;
    time_t tm_t = tp.tv_sec;
    newtime = localtime( &tm_t );
    newTm.setTime(newtime->tm_hour, newtime->tm_min,newtime->tm_sec, tp.tv_usec/1000UL);

    Datetime now;
    now.m_date = Date(newtime);
    now.m_time = newTm;

    return now;
}

Date& Datetime::date() 
{
    return m_date;
}

Time& Datetime::time()
{
    return m_time;
}

string Datetime::toString() const
{
    return format("Y-M-D H:I:S");
}

const char* Datetime::toCStr()
{
    strcpy(mBuffer, toString().c_str());
    return mBuffer;
}

string Datetime::format(string fmtStr) const
{
    string retString;
    formtDateTime(retString, fmtStr, getYear(), getMonth(), getDay(), getHour(), getMinute(), getSecond(), getMillSecond());
    return retString;
}

bool Datetime::setDatetime(int year, int month, int day, int hour, int minute, int second, int millSecond)
{
    bool r0 = Date::isValid(year,month,day);
    bool r1 = Time::isValid(hour,minute,second,millSecond);
    if (r0 && r1)
    {
        m_date.setDate(year,month,day);
        m_time.setTime(hour,minute,second,millSecond);
        return true;
    }
    return false;
}

bool Datetime::setDatetime(string fmt)
{
    Date tmp0;
    Time tmp1;
    if (tmp0.setDate(fmt) && tmp1.setTime(fmt))
    {
        m_date = tmp0;
        m_time = tmp1;
        return true;
    }
    return false;
}


int Datetime::compare(const Datetime& oneTime) const
{
    int d =  m_date - oneTime.m_date;
    if (d!=0)
        return d;
    else
        return m_time - oneTime.m_time;
}


Datetime& Datetime::operator=(const Datetime& oneTime)
{
    if (&oneTime != this)
    {
        m_date = oneTime.m_date;
        m_time = oneTime.m_time;
    }
    return *this;
}

Datetime& Datetime::operator+=(TInt64 seconds)
{
    int days = (int) ( seconds / (24*3600) );
    int leftSecond = seconds % (24*3600);
    m_date += days;

    if ( leftSecond+m_time.toSeconds() >= (24*3600) )
    {
        m_date ++;
        m_time += leftSecond - 24*3600;
    }
    else if ( leftSecond+m_time.toSeconds() <= 0 )
    {
        m_date --;
        m_time += leftSecond + 24*3600;
    }
    else
        m_time += leftSecond;

    return *this;
}

Datetime& Datetime::operator-=(TInt64 seconds)
{
    *this += (-1*seconds);
    return *this;
}

Datetime& Datetime::operator++()
{
    *this += 1;
    return *this;
}

Datetime Datetime::operator++(int)
{
    Datetime tm_(*this);
    tm_+= 1;
    *this = tm_;
    return tm_;
}

Datetime& Datetime::operator--()
{
    *this += -1;
    return *this;
}

Datetime Datetime::operator--(int)
{
    Datetime tm_(*this);
    tm_ += -1;
    *this = tm_;
    return tm_;
}

namespace aps
{

Datetime operator+(const Datetime& oneTime, TInt64 seconds)
{
    Datetime tm_(oneTime);
    tm_ += seconds;
    return tm_;
}

Datetime operator+(TInt64 seconds, const Datetime& oneTime)
{
    Datetime tm_(oneTime);
    tm_ += seconds;
    return tm_;
}

Datetime operator-(const Datetime& oneTime, TInt64 seconds)
{
    Datetime tm_(oneTime);
    tm_ += (-1*seconds);
    return tm_;
}

TInt64 operator-(const Datetime& oneTime, const Datetime& twoTime)
{
    TInt64 days =oneTime.m_date-twoTime.m_date;
    TInt64 seconds = oneTime.m_time-twoTime.m_time;
    if (seconds<0 && days>0)
    {
        seconds += 24*3600UL;
        days--;
    }
    return days*24*3600UL + seconds;
}

}

TInt64 Datetime::toUnixTimestamp(const Datetime& tm_)
{
    return tm_-Datetime(1970,1,1,0,0,0,0)-8*3600;
}

TInt64 Datetime::toUnixTimestamp() const
{
    return Datetime::toUnixTimestamp(*this);
}

TInt64 Datetime::getCurUnixTimestamp()
{
    //修正8小时时差
    return Datetime::toUnixTimestamp(Datetime::now());
}

Datetime& Datetime::addHours( int hours )
{
    *this += hours*3600;
    return *this;
}

Datetime& Datetime::addMinutes( int minutes )
{
    *this += minutes*60;
    return *this;
}

Datetime& Datetime::addSeconds( int seconds )
{
    *this += seconds;
    return *this;
}

Datetime& Datetime::addDays( int day )
{
    m_date.addDays(day);
    return *this;
}

Datetime& Datetime::addMonths( int month )
{
    m_date.addMonths(month);
    return *this;
}

Datetime& Datetime::addYears( int year )
{
    m_date.addYears(year);
    return *this;
}
