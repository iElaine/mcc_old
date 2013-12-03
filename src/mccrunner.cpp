/**
 * @file mccrunner.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <cassert>
#include "log.h"
#include "mccrunner.h"
#include "configparser.h"
#include "mcckeyboardmanager.h"
#include "mccresourcemanager.h"
#include <iostream>

using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace std;

/**
 * @brief constructor
 */
MCCRunner::MCCRunner() :
	mRunning(false)
{

}

/**
 * @brief destructor
 */
MCCRunner::~MCCRunner()
{
	mRunning = false;
}

bool LogInitializer::bInitialized = LogInitializer::init();

int
MCCRunner::run(int argc, char *argv[], const char *filename)
{
	// TODO initialize mcc system
	assert(LogInitializer::isInitialized());
	mConfig = new ConfigParser;
	if(mConfig ==  NULL)
	{
		InfoLog("failed to create configuration parser");
		return -1;
	}
	if(mConfig->Parse(argc, argv, filename) < 0)
	{
		InfoLog("failed to parse configuration");
		delete mConfig;
		return -1;
	}
	if(mConfig->getConfigBool("MCC", "Daemonize", false))
	{
		InfoLog("Daemonized!");
		Daemonize();
	}
	mResourceManager = new MCCResourceManager(*mConfig);
	if(mResourceManager ==  NULL)
	{
		InfoLog("failed to create resource manager");
		delete mConfig;
		return -1;
	}
	mResourceManager->init(mConfig);
	mKeyboardManager = new MCCKeyboardManager(*mConfig);
	int matrix_id = mConfig->getConfigInt("MCC", "MatrixID", 0);
	mKeyboardManager->SetMatrixId(matrix_id);
	if(mKeyboardManager ==  NULL)
	{
		delete mConfig;
		delete mResourceManager;
		return -1;
	}
	int rc;
	mKeyboardManager->SetResourceManager(mResourceManager);
	mResourceManager->setKeyboardManager(mKeyboardManager);
	rc = mKeyboardManager->Start();
	if(rc != 0)
		return rc;
	for(int i=0;i<4;i++)
	{
		char kb_name[100];
		sprintf(kb_name,"Keyboard%d",i);
		if(mConfig->getConfigBool(kb_name, "Enable", false))
		{
			char *name = mConfig->getConfigString(kb_name, "Name", "");
			char *sp_kb = mConfig->getConfigString(kb_name, "KeyboardPath", "");
			char *sp_mt = mConfig->getConfigString(kb_name, "MatrixPath", "");
			char *setup = mConfig->getConfigString(kb_name, "Setup", "");
			if(setup[0] == '\0')
			{
				WarnLog("Setup for "<<kb_name<<" not find, using 9600,8,1,n");
				rc = mKeyboardManager->LoadKeyboard(name, sp_kb, sp_mt, "9600,8,1,n", i);
			}
			else
				rc = mKeyboardManager->LoadKeyboard(name, sp_kb, sp_mt, setup, i);
//			rc = mKeyboardManager->LoadKeyboard(name, sp_kb, sp_mt, baudrate, databits, stopbits, parity[0], i);
			if(rc != 0)
				return rc;
			InfoLog("Keyboard { \""<<name<<"\" , \""<<sp_kb<<"\" , \""<<sp_mt<<"\" , \""<<setup<<"\" }");
		}
	}
	int client_mgr_port = mConfig->getConfigInt("Client", "ListenPort", 30000);
	mClientManager = new MCCClientManager(*mKeyboardManager);
	mResourceManager->setClientManager(mClientManager);
	mClientManager->Start(NULL, client_mgr_port);
	mRunning = true;
	return 0;
}

int 
MCCRunner::shutdown()
{
	if(mRunning)
	{
		delete mConfig;
		delete mResourceManager;
		delete mKeyboardManager;
	}
	mRunning = false;
	return 0;
}


int 
MCCRunner::createKeyboardManager()
{

	return 0;
}

int 
MCCRunner::createResourceManager()
{
	return 0;
}

int
MCCRunner::createClientManager()
{
	return 0;
}
