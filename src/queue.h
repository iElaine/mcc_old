/**
 * @file queue.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <cstdlib>
#include <list>
#include "autolock.h"
#include "exceptionbase.h"

template <class T>
class Queue {
	public:
		/**
		 * @brief constructor
		 */
		Queue() {}
		/**
		 * @brief destructor
		 */
		virtual ~Queue() {}

		inline int inQueue(T ele) {
			AutoLock lock(mMutex);
			mQueue.push_back(ele);
			return 0;
		}

		inline int deQueue(T &ele) {
			AutoLock lock(mMutex);
			if(mQueue.size() == 0)
				return -1;
			T res = mQueue.front();
			mQueue.pop_front();
			ele = res;
			return 0;
		}

		inline int Size() { return mQueue.size();}
	protected:
		std::list<T> mQueue;
		Mutex mMutex;
};
#endif /* __QUEUE_H__ */
