/**
 * @file threadpool.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__
#include <vector>
#include <memory>
#include "thread.h"
#include "squeue.h"
#include "autolock.h"
class Task {
	public:
		typedef enum TaskResult {
			CONTINUE,
			EXIT
		} TaskResult;
		virtual int operator()() = 0;
	protected:
		void *param;
};

class ThreadPool {
	public:
		// work thread
		class WorkThread : public ThreadIf {
			protected:
				friend class ThreadPool;
				WorkThread(Squeue<Task *>& fifo) : ThreadIf("WorkThread"), mTaskFifo(fifo) {}
				virtual void thread() {
					Task *t = NULL;
					while(!isStop())
					{
						mTaskFifo.get(t);
						assert(t);
						std::auto_ptr<Task> task(t);
						(*task)();
					}
				}
				Squeue<Task *> &mTaskFifo;
		};
		ThreadPool(int size) {
			WorkThread *pthread;
			for(int i=0;i<size;i++)
			{
				pthread = new WorkThread(this->mTaskQueue);
				assert(pthread);
				mThreads.push_back(pthread);
			}
		}
		virtual ~ThreadPool() {
			StopAll();
		}
		void submitTask(Task *task) {
			mTaskQueue.add(task);
		}
		int Start() {
			int cnt = 0;
			AutoLock g(mMutex);
			std::vector<WorkThread *>::iterator i;
			for(i=mThreads.begin(); i!=mThreads.end();i++)
			{
				if((*i)->Start())
					cnt++;
			}
			return cnt;
		}
		void StopAll()
		{
			AutoLock g(mMutex);
			std::vector<WorkThread *>::iterator i;
			while(!mThreads.empty())
			{
				mThreads.back()->Shutdown();
				mThreads.pop_back();
			}
//			for(i=mThreads.begin(); i!=mThreads.end();i++)
//			{
//				(*i)->Shutdown();
//				mThreads.erase(i);
//			}
		}
	protected:
		class NullTask : public Task {
			public:
				virtual int operator()() { return 0;}
		};
		Squeue<Task *>				mTaskQueue;
		std::vector<WorkThread *>	mThreads;
		Mutex						mMutex;
};
#endif /* __THREADPOOL_H__ */
