/**
 * @file log.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __LOG_H__
#define __LOG_H__
#include <sys/time.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/layout.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/fileappender.h>
#include <iostream>
#include <iomanip>

typedef enum ELogLevel {
	ELogLevel_Debug = 0,
	ELogLevel_Trace,
	ELogLevel_Info,
} ELogLevel;

#define USE_LOG4CPLUS

#if !defined(USE_LOG4CPLUS)
#define InfoLog(log_text) do { \
	struct timeval tv;\
	gettimeofday(&tv, NULL);\
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(10)<<tv.tv_sec<<':'<<std::setfill('0')<<std::setw(6)<<tv.tv_usec<<std::setfill(' ')<<" | "<<std::setw(30)<<__FILE__<<'|'<<std::setw(5)<<__LINE__<<"| "<<std::setw(20)<<__func__<<"| "<<log_text<<std::endl;\
} while(0)
#else
#define InfoLog(log_text) do { \
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("logger")); \
	LOG4CPLUS_INFO(logger, log_text); \
} while(0)

#define WarnLog(log_text) do { \
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("logger")); \
	LOG4CPLUS_WARN(logger, log_text); \
} while(0)

#define FatalLog(log_text) do { \
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("logger")); \
	LOG4CPLUS_FATAL(logger, log_text); \
} while(0)

#define ErrLog(log_text) do { \
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("logger")); \
	LOG4CPLUS_ERROR(logger, log_text); \
} while(0)
#endif

#define DebugLog(log_text) do { \
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("logger")); \
	LOG4CPLUS_DEBUG(logger, log_text); \
} while(0)

#define TracelLog(log_text) do { \
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("logger")); \
	LOG4CPLUS_TRACE(logger, log_text); \
} while(0)
class LogInitializer {
	public:
		static bool isInitialized() { return bInitialized;}
		static bool init() {
			using namespace log4cplus;
			using namespace log4cplus::helpers;
			using namespace std;
			BasicConfigurator config;
			config.configure();
			// set pattern
			string pattern = "%d{20%y/%m/%d %H:%M:%S.%q} %m%n";
			// create console appender
			SharedObjectPtr<Appender> _append(new ConsoleAppender());
			_append->setName("Console append");
			std::auto_ptr<Layout> _layout(new PatternLayout(pattern));
			_append->setLayout(_layout);
			// creaet file appender
			SharedObjectPtr<Appender> _fileappend(new FileAppender("mcc.log"));
			_fileappend->setName("File append");
			std::auto_ptr<Layout> _filelayout(new PatternLayout(pattern));
			_fileappend->setLayout(_filelayout);
			// add appenders
			Logger _logger = Logger::getInstance("logger");
			_logger.addAppender(_append);
			_logger.addAppender(_fileappend);
			// set level
			_logger.setLogLevel(ALL_LOG_LEVEL);
			return true;
		}
	protected:
		static bool bInitialized;
};
#endif /* __LOG_H__ */
