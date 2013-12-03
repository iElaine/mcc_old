/**
 * @file message.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MESSAGE_H__
#define __MESSAGE_H__
enum SenderType {
	KEYBOARD,
	KEYBOARDMANAGER,
	RESOURCEMANAGER,
	CLIENT,
	CLIENTMANAGER,
	OTHER
};
class Message {
	public:
		/**
		 * @brief constructor
		 */
		Message() : mSenderId(-1), mSenderType(OTHER) {}
		Message(int id, SenderType type) : mSenderId(id), mSenderType(type) {}
		/**
		 * @brief destructor
		 */
		virtual ~Message();
	protected:
		int			mSenderId;
		SenderType	mSenderType;
};
#endif /* __MESSAGE_H__ */
