#ifndef __MCCAGENTEVENTHANDLER_H__
#define __MCCAGENTEVENTHANDLER_H__
#include "mccagentmessage.h"

class MCCAgentServer;
class MCCAgentClient;

class MCCAgentServerEventHandler {
	public:
		virtual void onNewConnection(int fd, MCCAgentServer &server) = 0;
		virtual void onDisconnect(int fd, MCCAgentServer &server) = 0;
		virtual void onRegister(int fd, MCCAgentRequest &request, MCCAgentServer &server) = 0;
		virtual void onServerRxRequest(int fd, MCCAgentRequest &request, MCCAgentServer &server) = 0;
		virtual void onServerRxResponse(int fd, MCCAgentResponse &response, MCCAgentServer &server) = 0;
};

class MCCAgentClientEventHandler {
	public:
		virtual void onClientConnected(int fd) = 0;
		virtual void onClientDisconnected(int fd) = 0;
		virtual void onClientRxRequest(int fd, MCCAgentRequest &request, MCCAgentClient &client) = 0;
		virtual void onClientRxResponse(int fd, MCCAgentResponse &response, MCCAgentClient &client) = 0;
};
#endif
