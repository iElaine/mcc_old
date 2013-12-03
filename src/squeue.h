#ifndef __Squeue_H__
#define __Squeue_H__
#include "mutex.h"
#include "conditional.h"
#include "autolock.h"
#include <queue>
#include <errno.h>

template <class T>
class Squeue {
	public:
		Squeue() {}
		~Squeue() {}
		inline void add(const T &e)
		{
			AutoLock g(&mMutex);
			mQueue.push(e);
			mCond.Signal();
		}
		inline void get(T &e)
		{
			AutoLock g(&mMutex);
			while(mQueue.empty())
				mCond.Wait(mMutex);
			e = mQueue.front();
			mQueue.pop();
		}
		inline bool get(T &e, time_t timeout)
		{
			AutoLock g(&mMutex);
			int rc;
			while(mQueue.empty())
			{
				if((rc = mCond.Wait(mMutex, timeout)) == ETIMEDOUT)
					return false;
				else if(rc < 0)
					abort();
			}
			e = mQueue.front();
			mQueue.pop();
			return true;
		}
		inline bool tryget(T &e)
		{
			AutoLock g(&mMutex);
			if(mQueue.empty())
				return false;
			e = mQueue.front();
			mQueue.pop();
			return true;
		}
		inline size_t size()
		{
			AutoLock g(&mMutex);
			return mQueue.size();
		}
	protected:
		Mutex mMutex;
		Conditional mCond;
		std::queue<T> mQueue;
};

#endif /* __Squeue_H__ */

