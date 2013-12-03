#ifndef __LOCKABLE_H__
#define __LOCKABLE_H__
#include "mutex.h"
class Lockable {
	public:
		void Lock() { mutex.Lock();}
		void Unlock() { mutex.Unlock();}
	protected:
		Mutex mutex;
};
#endif
