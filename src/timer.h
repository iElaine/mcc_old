#ifndef __TIMER_H__
#define __TIMER_H__
#include "threadif.h"
#include "heap.h"
#include "threadpool.h"
#include <map>

#define TIMER_WORK_THREAD 4
class TimerHandler {
	virtual void operator()() = 0;
};

typedef unsigned long TimerId;
class TimerUtil : public ThreadIf {
	public:
		TimerUtil() : ThreadIf("TimerThread") {}
		static void Init();
		static void UnInit();
		static TimerId addTimer(time_t interval, std::auto_ptr<TimerHandler> &handler);
		static void cancelTimer(TimerId id);
	protected:
		enum State {
			UNCREATED,
			INITIALIZED,
			DEINITIALIZED
		};
		class TimerInternal {
			public:
				TimerId id;
				std::auto_ptr<TimerHandler> handler;
				time_t expires;
		};
		virtual int process(); 
		static State mState = UNCREATED;
		static TimerUtil *mThread;
		static ThreadPool *mPool;
		static Heap<TimerInternal> *mTimerHeap;
		static TimerId mGlobalId = 0;
};
#endif /* __TIMER_H__ */

