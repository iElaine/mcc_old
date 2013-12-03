/**
 * @file mcckeyboardmanager.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include "mcckeyboardmanager.h"
#include "mcckeyboard.h"
#include "exceptionbase.h"
#include "log.h"
#include <cstdio>
#include <assert.h>
#include <string.h>
using namespace std;
//using namespace __gnu_cxx;


MCCKeyboardFactoryManager *MCCKeyboardFactoryManager::mInstance = NULL;


FactoryInfo *MCCKeyboardFactory::info;


MCCKeyboardFactory *MCCKeyboardFactoryManager::GetFactory(const char *name)
{
	return m_factory_map[name];
}

FactoryInfo * MCCKeyboardFactoryManager::RegisterFactory(const char *name, MCCKeyboardFactory *factory)
{
	FactoryInfo *info = new FactoryInfo(name);
	string sname(name);
	m_factory_map[sname] = factory;
	InfoLog("register keyboard "<<name<<'['<<factory<<']');
	return info;
}
/**
 * @brief constructor
 */
MCCKeyboardManager::MCCKeyboardManager(const ConfigParser &config, int max_kb_number) :
	ThreadIf("Keyboard Manager Thread"),
	mConfig(config)
{
	mFactoryManager = MCCKeyboardFactoryManager::getInstance();
	m_max_keyboards = max_kb_number > MCC_MAX_KEYBOARD_NUMBER ? MCC_MAX_KEYBOARD_NUMBER : max_kb_number;
	memset(m_keyboards, 0, sizeof(MCCKeyboard *)*MCC_MAX_KEYBOARD_NUMBER);
	memset(m_keyboard_process, 0, sizeof(ProcessInfo *)*MCC_MAX_KEYBOARD_NUMBER);
	InfoLog("Create Keyboard Manager "<<this);
}

/**
 * @brief destructor
 */
MCCKeyboardManager::~MCCKeyboardManager()
{
	InfoLog("Destroyed Keyboard Manager "<<this);
	for(int i=0;i<m_max_keyboards;i++)
		UnloadKeyboard(i);
}

MCCKeyboard *MCCKeyboardManager::Create(const char *name)
{
	string sname(name);
	MCCKeyboardFactory *factory = mFactoryManager->GetFactory(name);
	InfoLog("get keyboard factory "<<factory);
	return factory->Create(this);
}

int MCCKeyboardManager::LoadKeyboard(
		const char *name, 
		const char *keyboard, 
		const char *matrix, 
		int baudrate, 
		int databits, 
		int stopbits, 
		char parity, 
		int local_id)
{
	if(local_id > m_max_keyboards - 1 || local_id < 0)
	{
		ErrLog("load keyboard "<<name<<" failed! : keyboard id > max id");
		return -1;
	}
	if(m_keyboards[local_id] != NULL)
	{
		ErrLog("load keyboard "<<name<<" failed! : keyboard id has been used");
		return -1;
	}
	MCCKeyboard *kb = this->Create(name);
	if(kb->Load(keyboard, matrix, baudrate, databits, stopbits, parity) < 0)
	{
		ErrLog("load keyboard "<<name<<" failed! : failed to load keyboard");
		return -1;
	}
	m_keyboards[local_id] = kb;
	kb->SetId(local_id);
	InfoLog("load keyboard "<<name<<'['<<kb<<"] successfully!");
	return 0;
}

int 
MCCKeyboardManager::LoadKeyboard(const char *name, 
		const char *keyboard, 
		const char *matrix, 
		const char *setup, 
		int local_id)
{
	if(local_id > m_max_keyboards - 1 || local_id < 0)
	{
		ErrLog("load keyboard "<<name<<" failed! : keyboard id > max id");
		return -1;
	}
	if(m_keyboards[local_id] != NULL)
	{
		ErrLog("load keyboard "<<name<<" failed! : keyboard id has been used");
		return -1;
	}
	MCCKeyboard *kb = this->Create(name);
	if(kb->Load(keyboard, matrix, setup) < 0)
	{
		ErrLog("load keyboard "<<name<<" failed! : failed to load keyboard");
		return -1;
	}
	m_keyboards[local_id] = kb;
	kb->SetId(local_id);
	InfoLog("load keyboard "<<name<<'['<<kb<<"] successfully!");
	return 0;

}

struct MCCKeyboardSetupParam {
	MCCKeyboardSetupParam(
			const char *pname,
			const char *pkeyboard,
			const char *pmatrix,	
			int br,
			int db,
			int sb,
			char pr,
			int local_id) : baudrate(br) , databits(db), stopbits(sb), parity(pr), id(local_id) {
		strcpy(name, pname);
		strcpy(keyboard, pkeyboard);
		strcpy(matrix, pmatrix);
	}
	void Dump() {
		InfoLog( "name     : "<<name<<endl<<\
				"keyboard : "<<keyboard<<endl<<\
				"matrix   : "<<matrix<<endl<<\
				"baudrate : "<<baudrate<<endl<<\
				"databits : "<<databits<<endl<<\
				"stopbits : "<<stopbits<<endl<<\
				"parity   : "<<parity<<endl<<\
				"id       : "<<id<<endl);
	}
	char name[256];
	char keyboard[256]; 
	char matrix[256]; 
	int baudrate; 
	int databits; 
	int stopbits; 
	char parity; 
	int id;
};
//
//static int _keyboard_process(void *param, int len, SocketPair *sock)
//{
//	MCCKeyboardSetupParam *setup = (MCCKeyboardSetupParam *)param; 
//	MCCKeyboardFactory *factory = MCCKeyboardFactoryManager::GetFactory(setup->name);
//	MCCKeyboard *kb = NULL;
//	if(factory != NULL)
//		kb = factory->Create();
//	if(kb != NULL)
//	{
//		kb->Load(
//				setup->keyboard,
//				setup->matrix,
//				setup->baudrate,
//				setup->databits,
//				setup->stopbits,
//				setup->parity);
//		kb->Unload();
//	}
//	return 0;
//}
//
int MCCKeyboardManager::Post(MCCKeyboardMessage &msg)
{
//	return mFifo.inQueue(msg);
	mFifo.add(msg);
	return 0;
}

//int MCCKeyboardManager::LoadKeyboardInNewProcess(
//		const char *name, 
//		const char *keyboard, 
//		const char *matrix, 
//		int baudrate, 
//		int databits, 
//		int stopbits, 
//		char parity, 
//		int local_id)
//{
//	if(local_id > m_max_keyboards - 1 || local_id < 0)
//	{
//		InfoLog("invalid keyboard id\n");
//		return -1;
//	}
//	if(m_keyboard_process[local_id] != NULL)
//	{
//		return -1;
//	}
//	MCCKeyboardSetupParam setup(name, keyboard, matrix, baudrate, databits, stopbits, parity, local_id);
//	setup.Dump();
//	ProcessInfo *info = ProcessFactory::CreateProcess("KeyboardProcess", _keyboard_process, &setup, 0);
//	if(info == NULL)
//		return -1;
//	m_keyboard_process[local_id] = info;
//	return 0;
//}

MCCKeyboard *MCCKeyboardManager::getKeyboard(int keyboard_id)
{
	return m_keyboards[keyboard_id];
}

int MCCKeyboardManager::UnloadKeyboard(int keyboard_id)
{
	if(m_keyboards[keyboard_id] == NULL)
		return -1;
	m_keyboards[keyboard_id]->Unload();
	delete m_keyboards[keyboard_id];
	m_keyboards[keyboard_id] = NULL;
	InfoLog("Unload keyboard");
	return 0;
}

void
MCCKeyboardManager::thread()
{
	MCCKeyboardMessage msg;
	while(!isStop())
	{
//		if(mFifo.deQueue(msg) == 0)
//		if(mFifo.get(msg, 1000))
		mFifo.get(msg);
		{
			if(m_handler != NULL)
			{
//				InfoLog("KeyboardManager handles a message from keyboard "<<msg.keyboard_id<<endl<<msg);
				if(msg.type == eKBMsg_SWITCH || msg.type == eKBMsg_Pin )
					m_handler->onSwitch(getKeyboard(msg.keyboard_id), msg);
				else if(msg.type == eKBMsg_PTZ)
					m_handler->onPtz(getKeyboard(msg.keyboard_id), msg);
			}
		}	
	}
}

void 
MCCKeyboardManager::SendSwitch(int kid, MCCKeyboardMessage& msg)
{
	MCCKeyboard *keyboard = m_keyboards[kid];
	if(keyboard != NULL)
		keyboard->SendSwitch(msg);
}

void 
MCCKeyboardManager::SendPtz(int kid, MCCKeyboardMessage& msg)
{
	MCCKeyboard *keyboard = m_keyboards[kid];
	if(keyboard != NULL)
		keyboard->SendPtz(msg);
}
