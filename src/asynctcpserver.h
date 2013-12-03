#ifndef __ASYNCTCPSERVER_H__
#define __ASYNCTCPSERVER_H__
#include "threadif.h"
#include "autolock.h"
#include <netinet/in.h>
#include <vector>
#include <memory>
class AsyncTCPServer; 
class AsyncTCPServerHandler {
	public:
		/**
		 * called whenever there is a new client connected
		 * @param fd socket for client
		 */
		virtual void onNewConnection(int fd) = 0;
		/**
		 * called whenever a socket is about to close don't
		 * close it in call back
		 * @param fd socket for client
		 */
		virtual void onDisconnect(int fd) = 0;
		/**
		 * called whenever a socket is readable
		 * @param fd socket for client
		 */
		virtual void onReadable(AsyncTCPServer &server, int fd, void *data, size_t len) = 0;
		/**
		 * called whenever a socket is writable
		 * @param fd socket for client
		 */
		virtual void onWriteble(AsyncTCPServer &server, int fd) = 0;
		/**
		 * called whenever a socket has error 
		 * @param fd socket for client
		 */
		virtual void onExcept(int fd, int err) = 0;
};

/**
 * This is a tcp connection manager and a tcp server
 */
class AsyncTCPServer : public ThreadIf {
	public:
		typedef enum State {
			CREATED = 0,
			INITIALIZED,
			STARTED,
			STOPED,
			DEINITIALIZED
		} State;
		AsyncTCPServer(AsyncTCPServerHandler *handler);
		virtual ~AsyncTCPServer();
		int Start(const char *bind_ip, unsigned short port);
		void Stop();
		int Send(int fd, const void *buf, int len);
		int Receive(int fd, void *buf, int len);
	protected:
//		int setNonblocking(int fd);
		void closeAll();
		virtual void thread();
		State									mState;
		int										mListenFd;
		int										mEPollFd;
		std::vector<int>						mConnFdList;
		Mutex									mConnFdListMutex;
		AsyncTCPServerHandler				   *mHandler;
		Mutex									mMutex;
};

#endif /* __ASYNCTCPSERVER_H__ */
