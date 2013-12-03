/**
 * @file mcckeyboardmessagehandler.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCKEYBOARDMESSAGEHANDLER_H__
#define __MCCKEYBOARDMESSAGEHANDLER_H__
class MCCKeyboard;
class MCCKeyboardMessageHandler {
	public:
		/**
		 * @brief constructor
		 */
		MCCKeyboardMessageHandler() {};
		/**
		 * @brief destructor
		 */
		virtual ~MCCKeyboardMessageHandler() {};
		virtual void onSwitch(MCCKeyboard *kb, MCCKeyboardMessage &msg) = 0;
		virtual void onPtz(MCCKeyboard *kb, MCCKeyboardMessage &msg) = 0;
};
#endif /* __MCCKEYBOARDMESSAGEHANDLER_H__ */
