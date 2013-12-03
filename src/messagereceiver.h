/**
 * @file messagereceiver.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MESSAGERECEIVER_H__
#define __MESSAGERECEIVER_H__
#include "squeue.h"
#include "message.h"
class MessageReceiver {
	public:
		/**
		 * @brief constructor
		 */
		MessageReceiver() {}
		/**
		 * @brief destructor
		 */
		virtual ~MessageReceiver() {}
		void post(auto_ptr<Message> &message) { return mFifo.add(message);}
	protected:
		Squeue<auto_ptr<Message> > mFifo;
};
#endif /* __MESSAGERECEIVER_H__ */
