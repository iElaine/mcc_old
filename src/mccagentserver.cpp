#include "mccagentserver.h"
#include "log.h"
#include <iostream>
#include <time.h>
#include <errno.h>
using namespace std;

MCCAgentServer::MCCAgentServer() : 
	mHandler(NULL), 
	mServer(NULL) 
{
}

MCCAgentServer::~MCCAgentServer()  
{
}

int 
MCCAgentServer::Send(int fd, MCCAgentRequest &request)
{
	char obuf[1024];
	size_t len;
	len = request.print(obuf, sizeof(obuf));
	len = mServer->Send(fd, obuf, len);
	if(len < 0)
		return 1;
	InfoLog("Send Request:"<<endl<<request);
	return 0;
}

int 
MCCAgentServer::Send(int fd, MCCAgentResponse &response)
{
	char obuf[1024];
	size_t len;
	len = response.print(obuf, sizeof(obuf));
	len = mServer->Send(fd, obuf, len);
	if(len < 0)
		return 1;
	InfoLog("Send Response:"<<endl<<response);
	return 0;
}

int
MCCAgentServer::Start(unsigned short port)
{
	mServer = new AsyncTCPServer(this);
	return mServer->Start(NULL, port);
}

void
MCCAgentServer::Stop()
{
	mServer->Stop();
}

void 
MCCAgentServer::onNewConnection(int fd) 
{
	mHandler->onNewConnection(fd, *this);
}

void 
MCCAgentServer::onDisconnect(int fd) 
{
	mHandler->onDisconnect(fd, *this);
}

void 
MCCAgentServer::onReadable(AsyncTCPServer &server, int fd, void *data, size_t len) 
{
	assert(len > 0);
	char *msgbuf = (char *)data;
	if(msgbuf[0] == 0)
		return;
	msgbuf[len] = 0;
	MCCAgentResponse response(msgbuf, 0);
	if(response.isValid())
	{
		InfoLog("Receive Response:"<<endl<<response);
		mHandler->onServerRxResponse(fd, response, *this);
		return;
	}
	MCCAgentRequest request(msgbuf, 0);
	InfoLog("Receive Request:"<<endl<<response);
	if(request.method() == REGISTER)
		mHandler->onRegister(fd, request, *this);
	else
		mHandler->onServerRxRequest(fd, request, *this);
}

void 
MCCAgentServer::onWriteble(AsyncTCPServer &server, int fd) 
{
}

void 
MCCAgentServer::onExcept(int fd, int err) 
{
	InfoLog("Error:"<<strerror(err));
}
