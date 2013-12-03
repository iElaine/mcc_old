#include <cassert>
#include "timer.h"
using namespace std;


TimerUtil::State
TimerUtil::mState = TimerUtil::UNCREATED;
TimerUtil *TimerUtil::mThread = NULL;
ThreadPool *TimerUtil::mPool = NULL;
Heap<TimerInternal> *mTimerHeap = NULL;


void 
TimerUtil::Init() 
{
	assert(mState == UNCREATED);
	mThread = new TimerUtil;
	assert(mThread);
	mTimerHeap = new Heap<TimerInternal>();
	assert(mTimerHeap);
	mPool = new ThreadPool(TIMER_WORK_THREAD);
	mPool->Start();
};

TimerId
TimerUtil::addTimer(time_t interval, auto_ptr<TimerHandler> &handler)
{

	mTimerHeap->push();
}
