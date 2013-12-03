#ifndef __THREAD_H__
#define __THREAD_H__
#include "threadif.h"
typedef int(*ThreadFunc)(void *param);

class Thread : public ThreadIf {
	public:
		Thread() : ThreadIf("Thread") {}
		int Start(ThreadFunc func, void *param) {
			mFunc = func;
			mParam = param;
			return ThreadIf::Start();
		}
	protected:
		virtual void thread() {
			mFunc(mParam);
		}
		ThreadFunc mFunc;
		void *mParam;
};
#endif
