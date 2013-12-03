/**
 * @file mccresourceprocessor.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCRESOURCEPROCESSOR_H__
#define __MCCRESOURCEPROCESSOR_H__
#include "mcckeyboardmessagehandler.h"
#include "mccresourcemanager.h"
class MCCResourceProcessor : MCCKeyboardMessageHandler{
	public:
		/**
		 * @brief constructor
		 */
		MCCResourceProcessor(MCCResourceManager &mgr);
		/**
		 * @brief destructor
		 */
		virtual ~MCCResourceProcessor();
		virtual int onSwitch(MCCKeyboard &kb, MCCKeyboardMessage &msg); 
		virtual int onPtz(MCCKeyboard &kb, MCCKeyboardMessage &msg);
	private:
		const MCCResourceManager &mResourceManager;
};
#endif /* __MCCRESOURCEPROCESSOR_H__ */
