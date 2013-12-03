#ifndef __MCCAGENTCLIENT_H__
#define __MCCAGENTCLIENT_H__
#include "mccagenteventhandler.h"
#include "asynctcpclient.h"
#include "mccagentmessage.h"
class MCCAgentClient : public AsyncTCPClientHandler {
	public:
		MCCAgentClient();
		void setEventHandler(MCCAgentClientEventHandler *handler) { mHandler = handler;}
		int Start(const char *server_ip, unsigned short port);
		void Stop();
		int Send(MCCAgentRequest &request);
		int Send(MCCAgentResponse &response);
	protected:
		virtual void onReadable(AsyncTCPClient &client, int fd, void *data, size_t len);
		virtual void onConnected(AsyncTCPClient &client, int fd);
		virtual void onDisconnected(AsyncTCPClient &client, int fd);
		virtual void onExcept(int fd, int err);
		MCCAgentClientEventHandler *mHandler;
		AsyncTCPClient *mClient;
};
#endif
