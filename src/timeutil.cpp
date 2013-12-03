/**
 * @file timeutil.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#include "timeutil.h"
using namespace std;

#define SEC_PER_DAY        86400
#define SEC_PER_HOUR    3600
#define DAY_PER_4YEAR    1461

/**
 * @brief constructor
 */
TimeUtil::TimeUtil()
{

}

/**
 * @brief destructor
 */
TimeUtil::~TimeUtil()
{

}
/**
 *  this file implement the convertion of seconds and date/_time
 */
typedef struct _time {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int msec;
	int usec;
} _time;


static const int __day_in_leapyear[13]        = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};
static const int __day_in_commonyear[13]    = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
static const int __day_in_4year[5]            = { 0, 365, 730, 1096, 1461};

static const _time ___timebase = { 1970, 1, 1, 0, 0, 0, 0 };

static int __memcpy(void *dst0, const void *src0, int n)
{
	char *src, *dst;
	src = (char *) src0;
	dst = (char *) dst0;
	while(n--)
	{
		*dst++ = *src++;
	}
	return 0;
}

static int SecToTime(long n_Sec, _time * p_Time)
{
	long t;
	int i;
	const int * day_in_year;
	if( n_Sec < 0 )
		return 0;
	__memcpy( p_Time, &___timebase, sizeof(_time));
	t = n_Sec % SEC_PER_DAY;
	p_Time->hour = t / SEC_PER_HOUR;
	t %= SEC_PER_HOUR;
	p_Time->minute = t / 60;
	p_Time->second = t % 60;

	t = n_Sec / SEC_PER_DAY;
	p_Time->year += 4 * (t/DAY_PER_4YEAR);
	t %= DAY_PER_4YEAR;
	for(i=0;i<4;i++)
	{
		if(t >= __day_in_4year[i] && t < __day_in_4year[i+1])
			break;
	}
	p_Time->year += i;
	if(i == 2)
		day_in_year = __day_in_leapyear;
	else
		day_in_year = __day_in_commonyear;
	t -= __day_in_4year[i];
	for(i=0;i<12;i++)
	{
		if(t >= day_in_year[i] && t < day_in_year[i+1])
			break;
	}
	p_Time->month += i;
	t -= day_in_year[i];
	p_Time->day += t;
	return 1;
}

static int TimeToSec(_time * p_Time)
{
	long res;
	long t;
	const int *day_in_year;
	if( p_Time->year % 4 == 0 )
		day_in_year = __day_in_leapyear;
	else
		day_in_year = __day_in_commonyear;
	res = 0;
	res += day_in_year[ p_Time->month - ___timebase.month] * SEC_PER_DAY;
	res += ( p_Time->day    - ___timebase.day    ) * SEC_PER_DAY;
	res += ( p_Time->hour    - ___timebase.hour    ) * SEC_PER_HOUR;
	res += ( p_Time->minute - ___timebase.minute ) * 60;
	res += ( p_Time->second - ___timebase.second );
	t = p_Time->year - ___timebase.year;
	res += t/4 * __day_in_4year[4] * SEC_PER_DAY;
	t %= 4;
	res += __day_in_4year[t] * SEC_PER_DAY;
	return res;
}

class Date {
	public:
};

class SysTime {
	public:
		SysTime()
		{
			m_time.tv_sec = 0;
			m_time.tv_usec = 0;
		}
		void Dump()
		{
			printf("System Time: %ld.%06ld\n", m_time.tv_sec,m_time.tv_usec);
		};

		void Dump2()
		{
			printf("UTC Time: %04d-%02d-%02d %02d:%02d:%02d.%03d.%03d \n",
			m_utc.year,
			m_utc.month,
			m_utc.day,
			m_utc.hour,
			m_utc.minute,
			m_utc.second,
			m_utc.msec,
			m_utc.usec);
		}
		void Update()
		{
			gettimeofday(&m_time, NULL);
			m_utc.msec = m_time.tv_usec / 1000;
			m_utc.usec = m_time.tv_usec % 1000;
			int days = m_time.tv_sec / SEC_PER_DAY;
			SecToTime(m_time.tv_sec, &m_utc);
		}
	protected:
		struct timeval m_time;
		_time m_utc;
		int _timezone;
};


void msleep(int msec)
{
	usleep(msec * 1000);
}
