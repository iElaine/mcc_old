#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__
#include <map>
#include <set>
#include "asynctcpserver.h"
#include "mutex.h"
class HttpServer : 
	public AsyncTCPServerHandler 
{
	public:
		int Start(int port);
	protected:
		virtual void onNewConnection(int fd);
		virtual void onDisconnect(int fd);
		virtual void onReadable(AsyncTCPServer &server, int fd, void *data, size_t len);
		virtual void onWriteble(AsyncTCPServer &server, int fd);
		virtual void onExcept(int fd,int err);
		AsyncTCPServer *mServer;
		std::set<int> mConnections;
};
#endif
