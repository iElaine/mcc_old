/**
 * @file mutex.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MUTEX_H__
#define __MUTEX_H__
#include <pthread.h>
#include <cassert>


class Mutex {
	public:
		/**
		 * @brief constructor
		 */
		Mutex() {
			mMutex = new pthread_mutex_t;
			assert(!pthread_mutex_init(mMutex, NULL));
		}
		/**
		 * @brief destructor
		 */
		virtual ~Mutex() {
			pthread_mutex_destroy(mMutex);
			delete(mMutex);
		}
		inline int Lock() {
			return pthread_mutex_lock(mMutex);
		}
		inline int Unlock() {
			return pthread_mutex_unlock(mMutex);
		}
		inline int TryLock() {
			return pthread_mutex_trylock(mMutex);
		}
		int Init();
	protected:
		friend class Semaphore;
		friend class Conditional;
		pthread_mutex_t *mMutex;
		pthread_mutexattr_t mAttr;
};
#endif /* __MUTEX_H__ */
