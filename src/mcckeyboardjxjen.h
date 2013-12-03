/**
 * @file mcckeyboardjxj.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCKEYBOARDJXJEN_H__
#define __MCCKEYBOARDJXJEN_H__
#include "mcckeyboard.h"
#include "mcckeyboardmessage.h"
#include <memory>
#define CHANNEL_NUM 16
#define CMD_LEN 8
class ThreadIf;
class MCCKeyboardJXJEn : public MCCKeyboard {
	public:
		/**
		 * @brief constructor
		 */
		MCCKeyboardJXJEn(MCCKeyboardManager *mgr);
		/**
		 * @brief destructor
		 */
		virtual ~MCCKeyboardJXJEn();
		virtual int Load(const char *keyboard, const char *matrix, int baudrate, int databits, int stopbits, char parity); 
		virtual int Unload();
		int Send(void *cmd, int len, int channel);
		virtual int SendSwitch(int local_cid, int local_tid); 
		virtual int SendPtz(int local_cid);
		virtual int SendSwitch(MCCKeyboardMessage &msg);
		virtual int SendPtz(MCCKeyboardMessage &msg);
		virtual int Notify(MCCKeyboardMessage &msg, int status_code, const char *reason);
	protected:
		bool ValidatePacket(char *packet, int len);
		virtual int buildMessageFromPacket(char *packet, int len, MCCKeyboardMessage &msg);
		virtual int buildPacketFromMessage(char *buf,    int len, MCCKeyboardMessage &msg);
		virtual int OnRxKeyboardPacket(SerialPort *sp);
		virtual int OnRxMatrixPacket(SerialPort *sp);
		unsigned char ibuff[CMD_LEN];
		unsigned char obuff[CMD_LEN];
		std::auto_ptr<ThreadIf> mQueryThread;
		SerialPort *m_matrix2;
		MCCKeyboardMessage mLastMessage;
};
#endif /* __MCCKEYBOARDJXJEN_H__ */
