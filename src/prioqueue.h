#ifndef __PRIOHEAP_H__
#define __PRIOHEAP_H__
#include "heap.h"
#include "autolock.h"
template <class T>
class PrioQueue {
	public:
		void add(const T &e) {
			AutoLock g(mMutex);
			mQueue.push(e);
		}
		bool tryget(T &e) {
			AutoLock g(mMutex);
			return mQueue.pop(e);
		}
		void get(T &e)
		{
			AutoLock g(mMutex);
			while(mQueue.empty())
				mCond.Wait(mMutex);
			assert(mQueue.pop(e));
		}
		size_t size()
		{
			return mQueue.size();
		}
	protected:
		Heap<T> mQueue;
		Mutex mMutex;
		Conditional mCond;
};
#endif /* __PRIOHEAP_H__ */
