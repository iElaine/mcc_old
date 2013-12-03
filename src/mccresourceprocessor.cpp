/**
 * @file mccresourceprocessor.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include "mccresourceprocessor.h"

/**
 * @brief constructor
 */
MCCResourceProcessor::MCCResourceProcessor(MCCResourceManager &mgr) :
	mResourceManager(mgr)
{

}

/**
 * @brief destructor
 */
MCCResourceProcessor::~MCCResourceProcessor()
{

}
int 
onSwitch(MCCKeyboard &kb, MCCKeyboardMessage &msg) 
{
	int res = mResourceManager.ProcessSwitchMessage(msg);
	return 0;
}

int 
onPtz(MCCKeyboard &kb, MCCKeyboardMessage &msg)
{
	int res = mResourceManager.ProcesMessage(msg);
	return 0;
}

