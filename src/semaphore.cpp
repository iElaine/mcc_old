/**
 * @file semaphore.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include "semaphore.h"
#include "mutex.h"

/**
 * @brief constructor
 */
Semaphore::Semaphore() 
{
	if(pthread_cond_init(&sem, NULL) != 0)
		throw "failed to init cond";
}

/**
 * @brief destructor
 */
Semaphore::~Semaphore()
{
	pthread_cond_destroy(&sem);
}

int Semaphore::Init()
{
	return 0;
}

int Semaphore::Wait(Mutex &mutex)
{
	return pthread_cond_wait(&sem, mutex.mMutex);
}

int Semaphore::TimedWait(Mutex &mutex, int timeout)
{
	return pthread_cond_wait(&sem, mutex.mMutex);
}

int Semaphore::Signal()
{
	return pthread_cond_signal(&sem);
}
