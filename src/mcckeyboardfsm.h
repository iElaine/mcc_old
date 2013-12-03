/**
 * @file mcckeyboardfsm.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCKEYBOARDFSM_H__
#define __MCCKEYBOARDFSM_H__
#include "mcckeyvalue.h"
class MCCKeyboardFSM {
	public:
		/**
		 * @brief constructor
		 */
		MCCKeyboardFSM();
		/**
		 * @brief destructor
		 */
		virtual ~MCCKeyboardFSM();
	protected:
		virtual MCCKeyboardMessage & onKeyin(eMCCKeyValue key);
};
#endif /* __MCCKEYBOARDFSM_H__ */
