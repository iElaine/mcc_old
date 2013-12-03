/**
 * @file semaphore.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
//#include "mutex.h"
#include <pthread.h>
class Mutex;
class Semaphore {
	public:
		/**
		 * @brief constructor
		 */
		Semaphore();
		/**
		 * @brief destructor
		 */
		virtual ~Semaphore();
		int Init();
		int Wait(Mutex&);
		int TimedWait(Mutex&, int);
		int Signal();
	public:
		pthread_cond_t sem;
		pthread_attr_t attr;
};
#endif /* __SEMAPHORE_H__ */
