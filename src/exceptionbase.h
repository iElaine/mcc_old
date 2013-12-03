/**
 * @file exceptionbase.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __EXCEPTIONBASE_H__
#define __EXCEPTIONBASE_H__
#define EXCEPTIONBASE(msg) ExceptionBase(__FILE__, __func__, __LINE__, msg)
#include <ostream>
#include <string>
class ExceptionBase {
	public:
		/**
		 * @brief constructor
		 */
		ExceptionBase(const char *file, const char *func, int line, const char *msg) : mFile(file), mFunc(func), mLine(line), mMsg(msg) {};
		/**
		 * @brief destructor
		 */
		virtual ~ExceptionBase() {};
	protected:
		int mErrno;
		std::string mFile;
		std::string mFunc;
		int mLine;
		std::string mMsg;
};

//std::ostream &operator<<(std::ostream &os, const ExceptionBase& exception);
#endif /* __EXCEPTIONBASE_H__ */
