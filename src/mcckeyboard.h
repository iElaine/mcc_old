/**
 * @file mcckeyboard.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCKEYBOARD_H__
#define __MCCKEYBOARD_H__
#include <map>
#include <string>
#include "serialport.h"
#include "socketpair.h"
#include "serialportlistener.h"
#include "mcckeyboardmessage.h"
#include "queue.h"
//#include "queue.h"
class SerialPacket {
	public:
		SerialPacket(const char *p_data, int len) : data(NULL), data_len(0) {
			data = new char[len];
			if(data == NULL)
			{
				throw "No Memory";
			}
			char *p = data;
			while(len--)
				*p++ = *p_data++;
		};
		~SerialPacket() { if(data) delete[] data; };
		char *data;
		int data_len;
};

class MCCKeyboardManager;
/**
 * MCCKeyboard is derived from SerialPortListener, which receive packet from wire
 * this class receive raw packet from wire, and generate keyboardmessage, then 
 * post it to keyboardmanager, the manager will handle the message
 */
class MCCKeyboard : public SerialPortListener
{
	public:
		/**
		 * @brief constructor
		 */
		MCCKeyboard(MCCKeyboardManager *kb);
		/**
		 * @brief destructor
		 */
		~MCCKeyboard();
		virtual bool isLocked() const { return mLocked;}
		virtual void lock() { mLocked = true;}
		virtual void unlock() { mLocked = false;}
		virtual int Load(const char *keyboard, const char *matrix, int baudrate, int databits, int stopbits, char parity); 
		virtual int Load(const char *keyboard, const char *matrix, const char *setup); 
		virtual int Unload();
		virtual int SendtoKeyboard(void *data, int len);
		virtual int SendtoMatrix(void *data, int len);
		virtual int Post(MCCKeyboardMessage &msg) { return mFifo.inQueue(msg);}
		virtual int SendSwitch(int local_cid, int local_tid) { return 0;}
		virtual int SendSwitch(MCCKeyboardMessage &msg) { return 0;}
		virtual int SendPtz(int local_cid) { return 0;}
		virtual int SendPtz(MCCKeyboardMessage &msg) { return 0;}
		virtual int Notify(MCCKeyboardMessage &msg, int status_code, const char *reason) { return 0;}
		void SetId(int id) { mId = id;}
		int SetSocketPair(SocketPair *s) { 
			if(s == NULL)
				return -1;
			m_sock = s;
			return 0;
		}
	protected:
		virtual int on_recv(SerialPort *sp);
		virtual int on_send(SerialPort *sp) { return 0;};
		virtual int on_error(SerialPort *sp) { return 0;};
		virtual int OnRxKeyboardPacket(SerialPort *sp) { return 0;};
		virtual int OnRxMatrixPacket(SerialPort *sp) { return 0;};
		SerialPort *GetKeyboardSp() { return m_keyboard;}
		SerialPort *GetMatrixSp() { return m_matrix;}
		SerialPort *m_keyboard;
		SerialPort *m_matrix;
		std::map<int, int> cached_cmd_map;
		int mId;
		int m_priority;
		int m_status;
		SocketPair *m_sock;
		MCCKeyboardManager *mMgr;
//		MCCKeyboardMessage mCurrentMessage;
		Queue<MCCKeyboardMessage> mFifo;
		bool mLocked;
};
#endif /* __MCCKEYBOARD_H__ */
