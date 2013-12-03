#include <cassert>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include "asynctcpclient.h"
#include "log.h"
using namespace std;

AsyncTCPClient::AsyncTCPClient(AsyncTCPClientHandler *handler) :
	ThreadIf("AsyncTCPClient"),
	mHandler(handler),
	mState(CREATED)
{
}

AsyncTCPClient::~AsyncTCPClient()
{
	Stop();
}

int
AsyncTCPClient::Start(const char *serverip, unsigned short port)
{
	if(mState == STARTED)
		return 1;
	strcpy(mServerIp, serverip);
	mPort = port;
	if(ThreadIf::Start())
		close(mFd);
	mState = STARTED;
	return 0;
}

void
AsyncTCPClient::Stop()
{
	if(mState != STARTED)
		return;
	ThreadIf::Shutdown();
//	ThreadIf::Stop();
	close(mFd);
	mState = DISCONNECTED;
}

void
AsyncTCPClient::thread()
{
	char msgbuf[2048];
	while(!isStop())
	{
		// init socket
		mFd = socket(AF_INET, SOCK_STREAM, 0);
		if(mFd < 0)
		{
			return;
		}
		struct sockaddr_in server_addr;
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(mPort);
		server_addr.sin_addr.s_addr = inet_addr(mServerIp);
		// connect to server
		int rc = connect(mFd, (struct sockaddr*)&server_addr, sizeof(server_addr));
		if(rc < 0)
		{
			close(mFd);
			mFd = -1;
			sleep(2);
			if(mHandler)
				mHandler->onExcept(mFd, rc);
			cout<<ECONNREFUSED<<","<<rc<<endl;
			continue;
		}
		mState = CONNECTED;
		InfoLog("asynctcpclient: connected to "<<mServerIp<<":"<<mPort);
		if(mHandler)
			mHandler->onConnected(*this, mFd);
		while(mState == CONNECTED)
		{
			fd_set set;
			FD_ZERO(&set);
			FD_SET(mFd, &set);
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 500000;
			int rc = select(mFd + 1, &set, NULL, NULL, &tv);
			if(rc == 0)
				continue;
			else if(rc < 0)
			{
				close(mFd);
				mState = DISCONNECTED;
				mHandler->onDisconnected(*this, mFd);
			}
			else
			{
				if(FD_ISSET(mFd, &set))
				{
					int len = recv(mFd, msgbuf, sizeof(msgbuf), 0);
					if(len == 0)
					{
						close(mFd);
						mState = DISCONNECTED;
						if(mHandler)
							mHandler->onDisconnected(*this, mFd);
						mFd = -1;
						break;
					}
					else if(len < 0)
					{
						if(mHandler)
							mHandler->onExcept(mFd, len);
						close(mFd);
					}
					if(mHandler)
						mHandler->onReadable(*this, mFd, msgbuf, len);
				}
			}
		}
	}
}

int 
AsyncTCPClient::Send(const void *buf, int len)
{
	if(mState != CONNECTED)
		return -1;
	return send(mFd, buf, len, 0);
}

int 
AsyncTCPClient::Receive(void *buf, int len)
{
	if(mState != CONNECTED)
		return -1;
	return recv(mFd, buf, len, 0);
}
