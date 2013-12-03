/**
 * @file configparser.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <cassert>
#include <cstring>
#include "configparser.h"
#include "log.h"

using namespace std;

/**
 * @brief constructor
 */
ConfigParser::ConfigParser()
{
	mCmdConfig = NULL;
	mFileConfig = NULL;
}

/**
 * @brief destructor
 */
ConfigParser::~ConfigParser()
{
	if(mCmdConfig)
	{
		ini_config_destroy(mCmdConfig); 
		mCmdConfig = NULL;
	}
	if(mFileConfig)
	{
		ini_config_destroy(mFileConfig); 
		mFileConfig = NULL;
	}
}

int ConfigParser::Parse(int argc, char *argv[], const char *filename)
{
	InfoLog("parse file "<<filename);
	assert(!mCmdConfig);
	assert(!mFileConfig);
	mFileConfig = ini_config_create_from_file(filename, 0);
	if(!mFileConfig)
		return -1;
	return 0;
}


char *ConfigParser::getConfigString(const char *section, const char *name, const char *default_string)
{
	if(mFileConfig == NULL)
		return NULL;
	return ini_config_get_string(mFileConfig, section, name, default_string);
}

int ConfigParser::getConfigInt(const char *section, const char *name, int default_int)
{
	if(mFileConfig == NULL)
		return -1;
	return ini_config_get_int(mFileConfig, section, name, default_int);
}

bool ConfigParser::getConfigBool(const char *section, const char *name, bool default_val)
{
	if(mFileConfig == NULL)
		return -1;
	return ini_config_get_int(mFileConfig, section, name, (int)default_val);
}

bool ConfigParser::getConfigIntArray(const char *section, const char *name, const char *splitstr, vector<string> &res)
{
	return false;
}
