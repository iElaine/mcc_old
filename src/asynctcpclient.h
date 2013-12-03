#ifndef __ASYNCTCPCLIENTHANDLER_H__
#define __ASYNCTCPCLIENTHANDLER_H__
#include "threadif.h"
#include "autolock.h"
class AsyncTCPClient;
class AsyncTCPClientHandler {
	public:
		virtual void onReadable(AsyncTCPClient& client, int fd, void *data, size_t len) = 0;
		virtual void onConnected(AsyncTCPClient& client, int fd) = 0;
		virtual void onDisconnected(AsyncTCPClient& client, int fd) = 0;
		virtual void onExcept(int fd, int err) = 0;
};

class AsyncTCPClient : public ThreadIf {
	public:
		typedef enum State {
			CREATED = 0,
			STARTED,
			CONNECTED,
			DISCONNECTED
		} State;
		AsyncTCPClient(AsyncTCPClientHandler *handler);
		virtual ~AsyncTCPClient();
		int Start(const char *serverip, unsigned short port);
		void Stop();
		int fd() { return mFd;}
		int Send(const void *buf, int len);
		int Receive(void *buf, int len);
	protected:
		virtual void thread();
		int mFd;
		AsyncTCPClientHandler *mHandler;
		unsigned short mPort;
		char mServerIp[256];
		State mState;
		Mutex mMutex;
};
#endif
