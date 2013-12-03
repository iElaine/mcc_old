#ifndef __Conditional_H__
#define __Conditional_H__
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "mutex.h"

class Mutex;
class Conditional {
	public:
		enum State{
			UNINITIALIZED = 0,
			INITIALIZED
		};
		Conditional() : mState(UNINITIALIZED) {
			mCond = new pthread_cond_t;
			assert(mCond);
			if(pthread_cond_init(mCond, NULL))
				abort();
			mState = INITIALIZED;
		}
		~Conditional() {
			int rc;
			if(mState == INITIALIZED && 
					(rc = pthread_cond_destroy(mCond)))
			{
				assert(rc == EBUSY);
				abort();
			}
		}
		inline int Wait(Mutex &mutex)
		{
			assert(mState == INITIALIZED);
			return pthread_cond_wait(mCond, mutex.mMutex);
		}
		inline int Wait(Mutex &mutex, time_t timeout)
		{
			assert(mState == INITIALIZED);
			struct timespec t;
			struct timeval tv;
			gettimeofday(&tv, NULL);
			t.tv_sec = tv.tv_sec + timeout/1e3;
			t.tv_nsec = tv.tv_usec*1e3 + timeout*1e6;
			t.tv_sec += t.tv_nsec/1e9;
			return pthread_cond_timedwait(mCond, mutex.mMutex, &t);
		}
		inline int Signal()
		{
			assert(mState == INITIALIZED);
			return pthread_cond_signal(mCond);
		}
	protected:
		pthread_cond_t *mCond;
		int mState;
};

#endif /* __Conditional_H__ */
