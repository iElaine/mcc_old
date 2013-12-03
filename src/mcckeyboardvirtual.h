/**
 * @file mcckeyboardvirtual.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCKEYBOARDVIRTUAL_H__
#define __MCCKEYBOARDVIRTUAL_H__

#include "mcckeyboardmanager.h"
#include "mcckeyboard.h"
#include "threadif.h"
#include <netinet/in.h>
#include <vector>
#include <memory>

class ClientInfo {
	public:
		ClientInfo(struct sockaddr_in &addr) : mAddr(addr) {}
		ClientInfo() {};
		struct sockaddr_in mAddr;
};

class MCCKeyboardVirtual : public MCCKeyboard, public ThreadIf {
	public:
		/**
		 * @brief constructor
		 */
		MCCKeyboardVirtual(MCCKeyboardManager *mgr);
		/**
		 * @brief destructor
		 */
		virtual ~MCCKeyboardVirtual();
		virtual int Load(const char *keyboard, const char *matrix, int baudrate, int databits, int stopbits, char parity);
		virtual int Unload();
		virtual int Notify(MCCKeyboardMessage &msg, int status_code, const char *reason);
		virtual void Update(MCCKeyboardMessage &msg);
	protected:
		virtual void thread();
		int PollClientFd();
	private:
		int buildFdSet(fd_set *fdset);
		int mListenFd;
		std::vector<int> mConnFdList;
		std::map<int, ClientInfo> mClientInfo; /* client id ==> client info */
		std::auto_ptr<ThreadIf> mListenThread;
};
#endif /* __MCCKEYBOARDVIRTUAL_H__ */
