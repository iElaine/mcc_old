#ifndef __UTIL_H__
#define __UTIL_H__
#include <string>
#include <vector>
class StringUtil {
	public:
		static int split(const std::string &srcStr, 
				const std::string &splitStr, 
				std::vector<std::string> &destVec);
		static int atoi(const std::string &str);
};
#endif
