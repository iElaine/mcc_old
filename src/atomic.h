#ifndef __ATOMIC_H__
#define __ATOMIC_H__
#include "autolock.h"
#include "rwlock.h"
template <class T>
class Atomic {
	public:
		Atomic(const T initVal) : mValue(initVal) {}
		Atomic() {}
		T &inc() {
			AutoWLock g(mLock);
			++mValue;
			return mValue;
		}
		T &inc(const T value) {
			AutoWLock g(mLock);
			mValue += value;
			return mValue;
		}
		T &dec() {
			AutoWLock g(mLock);
			++mValue;
			return mValue;
		}
		T &dec(const T value) {
			AutoWLock g(mLock);
			mValue -= value;
			return mValue;
		}
		T &operator++() {
			AutoWLock g(mLock);
			++mValue;
			return mValue;
		}
		T &operator--() {
			AutoWLock g(mLock);
			--mValue;
			return mValue;
		}
		T operator++(int)
		{
			AutoWLock g(mLock);
			T ret(mValue);
			++mValue;
			return ret;
		}
		T operator--(int)
		{
			AutoWLock g(mLock);
			T ret(mValue);
			--mValue;
			return ret;
		}
		T &get() {
			AutoRLock g(mLock);
			return mValue;
		}
		T &set(const T value) {
			AutoWLock g(mLock);
			mValue = value;
			return mValue;
		}
		T &operator=(const T value) {
			AutoWLock g(mLock);
			mValue = value;
			return mValue;
		}
	protected:
		T mValue;
		RWlock mLock;
};
#endif
