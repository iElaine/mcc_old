/**
 * @file mccrunner.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCRUNNER_H__
#define __MCCRUNNER_H__
#include "serverprocess.h"

class MCCKeyboardManager;
class MCCResourceManager;
class MCCClientManager;
class ConfigParser;

class MCCRunner : public ServerProcess {
	public:
		/**
		 * @brief constructor
		 */
		MCCRunner();
		/**
		 * @brief destructor
		 */
		virtual ~MCCRunner();
		int run(int argc, char *argv[], const char *filename);
		int shutdown();
		MCCKeyboardManager &KeyboardManager() { return *mKeyboardManager;}
	private:
		int createResourceManager();
		int createKeyboardManager();
		int createClientManager();
		MCCKeyboardManager  *mKeyboardManager;
		MCCResourceManager  *mResourceManager;
		ConfigParser		*mConfig;
		MCCClientManager	*mClientManager;
		bool				 mRunning;
};
#endif /* __MCCRUNNER_H__ */
