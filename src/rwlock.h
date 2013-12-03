#ifndef __RWLOCK_H__
#define __RWLOCK_H__
#include <pthread.h>
#include <stdexcept>
class RWlock {
	public:
		RWlock() {
			if(pthread_rwlock_init(&mRWlock, NULL))
				throw std::runtime_error("fail to initiate rwlock");
		}
		~RWlock() {
			pthread_rwlock_destroy(&mRWlock);
		}
		inline int Rdlock() {
			return pthread_rwlock_rdlock(&mRWlock);
		}
		int Wrlock() {
			return pthread_rwlock_wrlock(&mRWlock);
		}
		int Unlock() {
			return pthread_rwlock_unlock(&mRWlock);
		}
		inline int TryWrlock() {
			return pthread_rwlock_trywrlock(&mRWlock);
		}
		inline int TryRdlock() {
			return pthread_rwlock_tryrdlock(&mRWlock);
		}
	protected:
		RWlock(const RWlock&);
		RWlock& operator=(const RWlock&);
		pthread_rwlock_t mRWlock;
};
#endif
