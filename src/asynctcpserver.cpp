#include <cassert>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>
#include <cassert>
#include <signal.h>

#include "asynctcpserver.h"
#include "log.h"

using namespace std;

AsyncTCPServer::AsyncTCPServer(AsyncTCPServerHandler *handler) : 
	ThreadIf("AsyncTCPServer"), 
	mState(CREATED),
	mListenFd(-1)
{
	assert(handler);
	mHandler = handler;
//	sigset_t signal_mask;
//	sigemptyset (&signal_mask);
//	sigaddset (&signal_mask, SIGPIPE);
//	int rc = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
//	assert(rc == 0);
}

AsyncTCPServer::~AsyncTCPServer()
{
	Stop();
}

int 
AsyncTCPServer::Start(const char *bind_ip, unsigned short port) 
{
	int rc;
	// create epoll fd
	mEPollFd = epoll_create(1024);
	assert(mEPollFd);
	// create listen socket
	mListenFd = socket(AF_INET, SOCK_STREAM, 0);
	int opt =1;
	setsockopt(mListenFd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	if(mListenFd < 0)
		return -1;
	struct epoll_event e;
	e.data.fd = mListenFd;
	e.events = EPOLLIN | EPOLLET;
	epoll_ctl(mEPollFd, EPOLL_CTL_ADD, mListenFd, &e);
	// set socket to non-block mode
//	int flags = fcntl(mListenFd, F_GETFL, 0);
//	fcntl(mListenFd, F_SETFL, flags | O_NONBLOCK); 
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	if(bind_ip == NULL)
		server_addr.sin_addr.s_addr = INADDR_ANY;
	else
		server_addr.sin_addr.s_addr = inet_addr(bind_ip);
	/* bind address */
	if(bind(mListenFd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
	{
		close(mListenFd);
		mListenFd = -1;
		return -1;
	}
	/* listen on address */
	if(listen(mListenFd, 5))
	{
		close(mListenFd);
		mListenFd = -1;
		return -1;
	}
	/**
	 * start accept procedure and accepted connections receive thread 
	 */
	if((rc = ThreadIf::Start()) != 0)
		return rc;
	mState = STARTED;
	return 0;
}

void
AsyncTCPServer::Stop()
{
	if(mState == STOPED)
		return;
	ThreadIf::Shutdown();
//	ThreadIf::Stop();
	close(mListenFd);
	close(mEPollFd);
}

void
AsyncTCPServer::thread()
{	
	struct sockaddr_in client_addr;
	size_t len = sizeof(client_addr);
	struct epoll_event e, events[10];
	int event_len;
	char msgbuf[2048];
	while(!isStop())
	{
		event_len = epoll_wait(mEPollFd, events, 10, 500);
		for(int i=0;i<event_len;i++)
		{
			int sock = events[i].data.fd;
			if(sock == mListenFd)
			{
				int connfd = accept(mListenFd, (struct sockaddr*)&client_addr, &len);
				assert(connfd >= 0);
//				int flags = fcntl(connfd, F_GETFL, 0);
//				fcntl(connfd, F_SETFL, flags | O_NONBLOCK); 
				e.events = EPOLLIN | EPOLLHUP;
				e.data.fd = connfd;
				epoll_ctl(mEPollFd, EPOLL_CTL_ADD, connfd, &e);
				InfoLog("new client connected");
				if(mHandler)
					mHandler->onNewConnection(connfd);
			}
			else if(events[i].events & EPOLLIN)
			{
				if(sock < 0)
					continue;
				int len = recv(sock, msgbuf, sizeof(msgbuf), 0);
				if(len == 0)
				{
					InfoLog("client disconnected");
					epoll_ctl(mEPollFd, EPOLL_CTL_DEL, sock, &e);
					if(mHandler)
						mHandler->onDisconnect(sock);
					close(sock);
					continue;
				}
				else if(len < 0)
				{
					close(sock);
					if(mHandler)
						mHandler->onExcept(sock, len);
					continue;
				}
				if(mHandler)
					mHandler->onReadable(*this, sock, msgbuf, len);
			}
			else if(events[i].events & EPOLLHUP)
			{
				InfoLog("client disconnected");
				epoll_ctl(mEPollFd, EPOLL_CTL_DEL, sock, &e);
				if(mHandler)
					mHandler->onDisconnect(sock);
			}
		}
	}
}

int 
AsyncTCPServer::Send(int fd, const void *buf, int len)
{
	if(mState != STARTED)
		return -1;
	return send(fd, buf, len, 0);
}

int 
AsyncTCPServer::Receive(int fd, void *buf, int len)
{
	if(mState != STARTED)
		return -1;
	return recv(fd, buf, len, 0);
}

void 
AsyncTCPServer::closeAll()
{
	while(!mConnFdList.empty())
	{
		close(mConnFdList.back());
		mConnFdList.pop_back();
	}
}
