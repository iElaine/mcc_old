/**
 * @file mccclientmanager.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCCLIENTMANAGER_H__
#define __MCCCLIENTMANAGER_H__
#include "threadif.h"
#include "asynctcpserver.h"
#include <vector>
#include <set>

class MCCKeyboardManager;
class MCCKeyboardMessage;
class MCCClientManager : public AsyncTCPServerHandler{
	public:
		/**
		 * @brief constructor
		 */
		MCCClientManager(MCCKeyboardManager& mgr);
		/**
		 * @brief destructor
		 */
		virtual ~MCCClientManager();
		class ClientInfo {
			public:
				ClientInfo(int fd, int bindid) : mFd(fd), mBindKeyboardId(bindid) {}
			private:
				int mFd;
				int mBindKeyboardId;
		};
		int Start(const char *ip, unsigned short port);
		void Notify(MCCKeyboardMessage &msg);
	private:
		MCCKeyboardManager &mMgr;
		AsyncTCPServer		*mServer;
		std::set<int> mConnFdList;
		virtual void onNewConnection(int fd);
		virtual void onDisconnect(int fd);
		virtual void onExcept(int fd, int err);
		virtual void onReadable(AsyncTCPServer &server, int fd, void *data, size_t len);
		virtual void onWriteble(AsyncTCPServer &server, int fd);
};
#endif /* __MCCCLIENTMANAGER_H__ */
