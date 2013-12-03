/**
 * @file configparser.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __CONFIGPARSER_H__
#define __CONFIGPARSER_H__
#include <map>
#include <string>
#include "confile.h"
#include <vector>
class ConfigParser {
	public:
		/**
		 * @brief constructor
		 */
		ConfigParser();
		/**
		 * @brief destructor
		 */
		virtual ~ConfigParser();
		int Parse(int argc, char *argv[], const char *filename);
		char *getConfigString(const char *section, const char *name, const char *default_string);
		int getConfigInt(const char *section, const char *name, int default_int);
		bool getConfigBool(const char *section, const char *name, bool default_val);
		bool getConfigIntArray(const char *section, const char *name, const char *splitstr, std::vector<std::string> &res);
	private:
		INI_CONFIG *mCmdConfig;
		INI_CONFIG *mFileConfig;
};
#endif /* __CONFIGPARSER_H__ */
