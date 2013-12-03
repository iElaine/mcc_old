/**
 * @file mcckeyboardmanager.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCKEYBOARDMANAGER_H__
#define __MCCKEYBOARDMANAGER_H_
//#include <ext/hash_map>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include "mccresourcemanager.h"
#include "mcckeyboard.h"
#include "mcckeyboardmessage.h"
#include "mccclientmanager.h"
#include "mcckeyboardmessagehandler.h"
#include "configparser.h"
#include "processfactory.h"
#include "threadif.h"
#include "queue.h"
#include "squeue.h"
#define MCC_MAX_KEYBOARD_NUMBER 4

class MCCKeyboardFactory;

/**
 * MACRO REGISTER_KEYBOARD define a factory class which can create MCCKeyboard object
 * and register a object of factory to keyboard manager when c++ system is initializing
 */
#define REGISTER_KEYBOARD(name, keyboard_class) \
	class keyboard_class##Factory : public MCCKeyboardFactory { \
		public: \
			virtual MCCKeyboard *Create(MCCKeyboardManager *mgr) { \
				return (MCCKeyboard *)new keyboard_class(mgr); \
			} \
			virtual FactoryInfo GetInfo() { \
				return *info; \
			} \
			static FactoryInfo *info; \
	}; \
FactoryInfo *keyboard_class##Factory::info = (MCCKeyboardFactoryManager::getInstance())->RegisterFactory(name, (MCCKeyboardFactory *)new keyboard_class##Factory)

class FactoryInfo {
	public:
		FactoryInfo(const char *p) { 
			strcpy(name, p);
		}
		char name[100];
};

class MCCKeyboardFactory {
	public:
		virtual MCCKeyboard *Create(MCCKeyboardManager *mgr) = 0 ;//{ return new MCCKeyboard();}
		virtual FactoryInfo GetInfo() { return *info;};
		static FactoryInfo *info;
};

class MCCKeyboardFactoryManager {
	public:
		MCCKeyboardFactory *GetFactory(const char *name);
		FactoryInfo *RegisterFactory(const char *name, MCCKeyboardFactory *factory);
		static MCCKeyboardFactoryManager *getInstance() {
			if(mInstance == NULL)
				mInstance = new MCCKeyboardFactoryManager;
			return mInstance;
		}
	protected:
		std::map<std::string, MCCKeyboardFactory *> m_factory_map;
		static MCCKeyboardFactoryManager *mInstance;
	private:
		MCCKeyboardFactoryManager() {}
		MCCKeyboardFactoryManager(const MCCKeyboardFactoryManager &) {}
};

class MCCKeyboardManager : public ThreadIf {
	public:
		/**
		 * @brief constructor
		 */
		MCCKeyboardManager(const ConfigParser &config, int max_kb_number = MCC_MAX_KEYBOARD_NUMBER);
		/**
		 * @brief destructor
		 */
		virtual ~MCCKeyboardManager();
		int LoadKeyboard(const char *name, const char *keyboard, const char *matrix, int baudrate, int databits, int stopbits, char parity, int local_id);
		int LoadKeyboard(const char *name, const char *keyboard, const char *matrix, const char *setup, int local_id);
		void SetMatrixId(int matrix_id) { mMatrixId = matrix_id;}
		int GetMatrixId() { return mMatrixId;}
	 	int UnloadKeyboard(int keyboard_id);
		MCCKeyboardMessage *GetMessage();
		void SetMessageHandler(MCCKeyboardMessageHandler *handler) { m_handler = handler;};
		void SetResourceManager(MCCResourceManager *res_mgr) { m_handler = (MCCKeyboardMessageHandler *)res_mgr;}
		void SetClientManager(MCCClientManager *cli_mgr) { mClientManager = cli_mgr;}
		int Post(MCCKeyboardMessage &msg);
		MCCKeyboard& GetKeyboard(int id) { return *m_keyboards[id];}
		MCCKeyboard					*getKeyboard(int keyboard_id);
		void SendSwitch(int kid, MCCKeyboardMessage& msg);
		void SendPtz(int kid, MCCKeyboardMessage& msg);
	protected:
		virtual void				 thread();
		void						 DispatchMessage(const MCCKeyboardMessage &msg);
		MCCKeyboard					*Create(const char *name);
		virtual void				 onRxKeyboardMessage(MCCKeyboardMessage &msg) {};
		MCCKeyboard					*m_keyboards[MCC_MAX_KEYBOARD_NUMBER];
		ProcessInfo					*m_keyboard_process[MCC_MAX_KEYBOARD_NUMBER];	
		MCCResourceManager			*mResourceManager;
		MCCClientManager			*mClientManager;
		int							 m_max_keyboards;
		MCCKeyboardMessageHandler	*m_handler;
//		Queue<MCCKeyboardMessage>	 mFifo;
		Squeue<MCCKeyboardMessage>	 mFifo;
		const ConfigParser&			 mConfig;
		MCCKeyboardFactoryManager	*mFactoryManager;
		int							 mMatrixId;
};
 
#endif /* __MCCKEYBOARDMANAGER_H__ */
