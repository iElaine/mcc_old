/**
 * @file asyncprocessor.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __ASYNCPROCESSOR_H__
#define __ASYNCPROCESSOR_H__
#include "threadif.h"
class AsyncProcessor : public ThreadIf{
	public:
		/**
		 * @brief constructor
		 */
		AsyncProcessor();
		/**
		 * @brief destructor
		 */
		virtual ~AsyncProcessor();
};
#endif /* __ASYNCPROCESSOR_H__ */
