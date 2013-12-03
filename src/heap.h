#ifndef __HEAP_H__
#define __HEAP_H__
#include <algorithm>
#include <iostream>
#include <vector>

template <class T>
class Heap {
	public:
		Heap() {
			make_heap(mData.begin(), mData.end());
		}
		void push(const T &e) {
			mData.push_back(e);
			push_heap(mData.begin(), mData.end());
		}
		bool top(T &e)
		{
			if(!mData.empty())
			{
				e = mData.back();
				return true;
			}
			return false;
		}
		bool pop(T &e)
		{
			if(!mData.empty())
			{
				pop_heap(mData.begin(), mData.end());
				e = mData.back();
				mData.pop_back();
				return true;
			}
			return false;
		}
		inline size_t size(){ return mData.size();}
		inline bool empty() { return mData.empty();}
	protected:
		std::vector<T> mData;
};
#endif /* __HEAP_H__ */
