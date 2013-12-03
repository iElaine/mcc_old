#include "mccagentclient.h"
#include <cassert>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "log.h"
using namespace std;

MCCAgentClient::MCCAgentClient() : 
	mHandler(NULL), 
	mClient(NULL)
{
}

int MCCAgentClient::Start(const char *server_ip, unsigned short port)
{
	assert(mHandler);
	if(!mClient)
		mClient = new AsyncTCPClient(this);
	return mClient->Start(server_ip, port);
}

void
MCCAgentClient::Stop()
{
	mClient->Stop();
}

int MCCAgentClient::Send(MCCAgentRequest &request)
{
	char obuf[1024];
	size_t len;
	len = request.print(obuf, sizeof(obuf));
	len = mClient->Send(obuf, len);
	if(len < 0)
		return 1;
	InfoLog("Send Request:"<<endl<<request);
	return 0;
}

int MCCAgentClient::Send(MCCAgentResponse &response)
{
	char obuf[1024];
	size_t len;
	len = response.print(obuf, sizeof(obuf));
	len = mClient->Send(obuf, len);
	if(len < 0)
		return 1;
	InfoLog("Send Response:"<<endl<<response);
	return 0;
}

void MCCAgentClient::onReadable(AsyncTCPClient &client, int fd, void *data, size_t len) 
{
	char *msgbuf = (char *)data;
	assert(len > 0);
	if(msgbuf[0] == 0)
		return;
	msgbuf[len] = '\0';
	MCCAgentRequest request(msgbuf, len);
	if(request.isValid() && mHandler)
	{
		InfoLog("Receive Request"<<endl<<request);
		mHandler->onClientRxRequest(fd, request, *this);
		return;
	}
	MCCAgentResponse response(msgbuf, 0);
	if(mHandler)
	{
		InfoLog("Receive Response"<<endl<<response);
		mHandler->onClientRxResponse(fd, response, *this);
	}
}

void MCCAgentClient::onConnected(AsyncTCPClient &client, int fd) {
	if(mHandler)
		mHandler->onClientConnected(fd);
}

void MCCAgentClient::onDisconnected(AsyncTCPClient &client, int fd) {
	if(mHandler)
		mHandler->onClientDisconnected(fd);
}

void 
MCCAgentClient::onExcept(int fd, int err)
{
	InfoLog("Error:"<<strerror(errno));
}
