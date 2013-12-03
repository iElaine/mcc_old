/**
 * @file thread.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __THREADIF_H__
#define __THREADIF_H__
#include <string>
#include <pthread.h>
#include "autolock.h"
class ThreadIf {
	public:
		/**
		 * @brief constructor
		 */
		typedef enum State {
			CREATED = 0,
			INITIALIZED,
			RUNNING,
			STOPED
		} State;
		ThreadIf(const char *name = "anonymous");
		/**
		 * @brief destructor
		 */
		virtual ~ThreadIf();
		int Start();
		void Stop();
		void StopNoblock() { mbStop = true;}
		int Shutdown();
		void ShutdownNonblock();
		int Join();
		int Resume();
		int Suspend();
		int Detach();
		bool isRunning();
		bool isStop() { return mbStop;}
		const std::string &Name() { return mName;}
	protected:
		virtual void thread() = 0;
		virtual void atexit();
		bool mbStop;
		State mState;
		const std::string mName;
		pthread_t mTid;
		bool mbDetached;
		Mutex mMutex;
		static void *thread_shell(void *param);
		static void atexit_shell(void *param);
};
#endif /* __THREADIF_H__ */
