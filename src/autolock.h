/**
 * @file autolock.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __AUTOLOCK_H__
#define __AUTOLOCK_H__
#include "mutex.h"
#include "rwlock.h"
#include <cassert>

class AutoLock {
	public:
		/**
		 * @brief constructor
		 */
		AutoLock(Mutex *mutex) : m_mutex(*mutex) {
			assert(mutex);
			m_mutex.Lock();
		}
		AutoLock(Mutex &mutex) : m_mutex(mutex) {
			m_mutex.Lock();
		}
		/**
		 * @brief destructor
		 */
		~AutoLock() {
				m_mutex.Unlock();
		}
	private:
		Mutex &m_mutex;
};

class AutoRLock {
	public:
		AutoRLock(RWlock *rwlock) : mLock(*rwlock) {
			assert(rwlock);
			mLock.Rdlock();
		}
		AutoRLock(RWlock& rwlock) : mLock(rwlock) {
			mLock.Rdlock();
		}
		~AutoRLock() {
			mLock.Unlock();
		}
	private:
		RWlock& mLock;
};

class AutoWLock {
	public:
		AutoWLock(RWlock *rwlock) : mLock(*rwlock) {
			assert(rwlock);
			mLock.Wrlock();
		}
		AutoWLock(RWlock& rwlock) : mLock(rwlock) {
			mLock.Wrlock();
		}
		~AutoWLock() {
			mLock.Unlock();
		}
	private:
		RWlock& mLock;
};
#endif /* __AUTOLOCK_H__ */
