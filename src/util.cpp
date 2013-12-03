#include "util.h"
#include <string.h>
#include <iostream>
using namespace std;

int 
StringUtil::split(const string &srcStr, 
		const string &splitStr, 
		vector<string> &destVec)
{
	if(srcStr.size() == 0)
	{
		return 0;
	}
	size_t oldPos = 0, newPos = 0;
	string tempData;
	while(1)
	{
		newPos = srcStr.find(splitStr,oldPos);
		if(newPos!=string::npos)
		{
			tempData = srcStr.substr(oldPos, newPos - oldPos);
			destVec.push_back(tempData);
			oldPos=newPos+splitStr.size();
		}
		else if (oldPos <= srcStr.size())
		{
			tempData = srcStr.substr(oldPos);
			destVec.push_back(tempData);
			break;
		}
		else
		{
			break;
		}
	}
	return 0;
}

int
StringUtil::atoi(const string &str)
{
	size_t len = str.size();
	int res = 0;
	const char *buf = str.c_str();
	for(int i=0;i<len;++i)
	{
		if(buf[i] < '0' || buf[i] > '9')
			return 0;
		res *= 10;
		res += buf[i] - '0';
	}
	return res;
}
