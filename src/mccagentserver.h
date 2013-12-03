#ifndef __MCCAGENTSERVER_H__
#define __MCCAGENTSERVER_H__
#include "threadif.h"
#include "mccagentmessage.h"
#include "autolock.h"
#include "asynctcpserver.h"
#include "mccagenteventhandler.h"

class MCCAgentServer : public AsyncTCPServerHandler {
	public:
		MCCAgentServer();
		~MCCAgentServer();
		void setEventHandler(MCCAgentServerEventHandler *handler) { mHandler = handler;}
		int Start(unsigned short port);
		void Stop();
		int Send(int fd, MCCAgentRequest &request);
		int Send(int fd, MCCAgentResponse &response);
	protected:
		virtual void onNewConnection(int fd);
		virtual void onDisconnect(int fd);
		virtual void onReadable(AsyncTCPServer &server, int fd, void *data, size_t len);
		virtual void onWriteble(AsyncTCPServer &server, int fd);
		virtual void onExcept(int fd, int err);
		MCCAgentServerEventHandler *mHandler;
		AsyncTCPServer *mServer;
};
#endif /*__MCCAGENTSERVER_H__*/
