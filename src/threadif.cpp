/**
 * @file thread.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <cassert>
#include <stdexcept>
#include "cstdio"
#include "threadif.h"
#include "log.h"
using namespace std;


void *
ThreadIf::thread_shell(void *param)
{
	ThreadIf *th = (ThreadIf *)param;
	InfoLog(th->mName<<"["<<th<<"] started");
	pthread_cleanup_push(atexit_shell, param);
	th->thread();
	pthread_cleanup_pop(0);
	InfoLog(th->mName<<"["<<th<<"] exit");
	th->mState = STOPED;
	return NULL;
}

void
ThreadIf::atexit_shell(void *param)
{
	ThreadIf *th = (ThreadIf *)param;
	th->atexit();
}


/**
 * @brief constructor
 */
ThreadIf::ThreadIf(const char *name) : 
	mbStop(false),
	mState(CREATED),
	mName(name),
	mTid(0),
	mbDetached(false)
{
}

/**
 * @brief destructor
 */
ThreadIf::~ThreadIf()
{
	Stop();
}

int ThreadIf::Start()
{
	int rc;
	AutoLock g(mMutex);
	if(mState == RUNNING)
		return 1;
	rc = pthread_create(&mTid, NULL, thread_shell, this);
	if(rc != 0)
		return rc;
	mState = RUNNING;
	return 0;
}

void
ThreadIf::Stop()
{
	AutoLock g(mMutex);
	if(mState != RUNNING)
		return;
	mbStop = true;
	Join();
}

void
ThreadIf::ShutdownNonblock()
{
	AutoLock g(mMutex);
	if(mState != RUNNING)
		return;
	pthread_cancel(mTid);
}

int
ThreadIf::Shutdown()
{
	AutoLock g(mMutex);
	if(mState != RUNNING)
		return 0;
	pthread_cancel(mTid);
	pthread_join(mTid, NULL);
	mState = STOPED;
	return 0;
}

int ThreadIf::Join()
{
	int rc;
	if(!(rc = pthread_join(mTid, NULL)))
		mbStop = false;
	return rc;
}

void 
ThreadIf::atexit()
{
	InfoLog(mName<<"["<<this<<"] exit");
}

int ThreadIf::Resume()
{
//	printf("Resume hasn't implement yet!\n")
	return 0;
}

int ThreadIf::Suspend()
{
//	printf("Suspend hasn't implement yet!\n")
	return 0;
}

int ThreadIf::Detach()
{
	return pthread_detach(mTid);
}
