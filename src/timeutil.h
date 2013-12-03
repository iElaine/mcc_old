/**
 * @file timeutil.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __TIMEUTIL_H__
#define __TIMEUTIL_H__
class Time {
	public:
		int Year() { return m_utc.year;}
		int Month() { return m_utc.month;}
		int Day() { return m_utc.day;}
		int Hour() { return m_utc.hour;}
		int Minute() { return m_utc.minute;}
		int Second() { return m_utc.second;}
		int mSecond() { return m_utc.msec;}
		int uSecond() { return m_utc.usec;}
	protected:
		struct timeval tv;
		struct _time m_utc;
};

class TimeUtil {
	public:
		/**
		 * @brief constructor
		 */
		TimeUtil();
		/**
		 * @brief destructor
		 */
		virtual ~TimeUtil();
		static long getTimestamp();
		static long getTimeofday();
};
#endif /* __TIMEUTIL_H__ */
