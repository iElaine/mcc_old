/**
 * @file mcckeyboardvirtual.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include "mcckeyboardvirtual.h"
#include "log.h"
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include "testclient/mccclient.h"

using namespace std;
using namespace mcc;

REGISTER_KEYBOARD("virtual", MCCKeyboardVirtual);


/**
 * @brief constructor
 */
MCCKeyboardVirtual::MCCKeyboardVirtual(MCCKeyboardManager *mgr) : 
	MCCKeyboard(mgr), 
	ThreadIf("Keyboard Virtual Thread")
{
	mListenFd = -1;
}

/**
 * @brief destructor
 */
MCCKeyboardVirtual::~MCCKeyboardVirtual()
{
	if(mListenFd > 0)
		close(mListenFd);
}

int 
MCCKeyboardVirtual::Load(const char *keyboard, 
		const char *matrix, 
		int baudrate, int databits, int stopbits, char parity)
{
	keyboard = keyboard;
	matrix = matrix;
	baudrate = baudrate;
	databits = databits;
	stopbits = stopbits;
	parity = parity;
	mListenFd = socket(AF_INET, SOCK_STREAM, 0);
	int opt =1;
	setsockopt(mListenFd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	if(mListenFd < 0)
		return -1;
	int flags = fcntl(mListenFd, F_GETFL, 0);
    fcntl(mListenFd, F_SETFL, flags | O_NONBLOCK); 
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(10000);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(mListenFd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
	{
		close(mListenFd);
		mListenFd = -1;
		return -1;
	}
	if(listen(mListenFd, 5))
	{
		close(mListenFd);
		mListenFd = -1;
		return -1;
	}
	InfoLog("start service on port 10000");
	return Start();
}

int 
MCCKeyboardVirtual::Unload()
{
	Stop();
	return 0;
}

int 
MCCKeyboardVirtual::Notify(MCCKeyboardMessage &msg, int status_code, const char *reason)
{
	InfoLog(msg);
	InfoLog("status:"<<status_code<<"  reason:"<<reason);
	return 0;
}

int 
MCCKeyboardVirtual::buildFdSet(fd_set *fdset)
{
	int maxfd = -1;
	vector<int>::iterator i;
	FD_ZERO(fdset);
	for(i = mConnFdList.begin(); i != mConnFdList.end(); i++)
	{
		if(*i > maxfd)
			maxfd = *i;
		FD_SET(*i, fdset);
	}
	return maxfd;
}

int 
MCCKeyboardVirtual::PollClientFd()
{
	int handled = 0;
	fd_set mFdSet;
	FD_ZERO(&mFdSet);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	// build fdset
	int maxfd = buildFdSet(&mFdSet);
	if(maxfd <= 0)
		return 0;
	if(select(maxfd + 1, &mFdSet, NULL, NULL, &tv) == 0)
		return 0;
	vector<int>::iterator i;
	MCCKeyboardMessage msg;
	InfoLog("max fd:"<<maxfd);
	for(i = mConnFdList.begin(); i != mConnFdList.end(); i++)
	{
		if(FD_ISSET(*i, &mFdSet))
		{
			InfoLog("handle a client event");
			handled++;
			client_packet packet;
			if(recv(*i, &packet, sizeof(client_packet), 0) == 0)
			{
				InfoLog("remote client disconnected!");
				close(*i);
				mConnFdList.erase(i);
				return 0;
			}
			switch(packet.type)
			{
				case -1:
					close(*i);
					mConnFdList.erase(i);
					break;
				case 1:
					msg.type = packet.type;
					msg.keyboard_id = packet.bind_kb;
					msg.camera_id = MCCGlobalID(packet.matrix_id, packet.cid);
					msg.monitor_id = packet.mid;
					mMgr->Post(msg);
					break;
				case 2:
					msg.type = packet.type;
					msg.keyboard_id = packet.bind_kb;
					msg.camera_id = MCCGlobalID(packet.matrix_id, packet.cid);
					msg.monitor_id = packet.mid;
					msg.ptz_opt_type = packet.ptz_type;
					if(msg.ptz_opt_type & 0x03)
						msg.ptz_opt_vstep = 20;
					else
						msg.ptz_opt_vstep = 0;
					if(msg.ptz_opt_type & 0x0c)
						msg.ptz_opt_hstep = 20;
					else
						msg.ptz_opt_hstep = 0;
					mMgr->Post(msg);
					break;
				default:
					break;
			}
			send(*i, &packet, sizeof(client_packet), 0);
		}
	}
	return handled;
}

void
MCCKeyboardVirtual::Update(MCCKeyboardMessage &msg)
{
	vector<int>::iterator i;
	static client_packet last_packet;
	client_packet packet;
	int len;
	packet.type = msg.type;
	packet.matrix_id = 0;
	packet.bind_kb = msg.keyboard_id;
	packet.cid = msg.camera_id.GetLocalID();
	packet.mid = msg.monitor_id;
	packet.ptz_type = msg.ptz_opt_type;
	packet.ptz_start = (msg.ptz_opt_type == 0) ? 0 : 1;
	if(last_packet.type == 2 &&
			packet.type == 2 &&
			last_packet.ptz_type == packet.ptz_type &&
			last_packet.ptz_start == packet.ptz_start)
	{
		return;
	}
	last_packet = packet;
	for(i = mConnFdList.begin(); i!=mConnFdList.end();++i)
	{
		len = send(*i, &packet, sizeof(client_packet), 0);
		if((len < 0 && errno == ECONNRESET) || len == 0)
		{
			close(*i);
			mConnFdList.erase(i);
		}
	}
}

void
MCCKeyboardVirtual::thread()
{
	MCCKeyboardMessage msg;
	struct sockaddr_in client_addr;
	size_t len = sizeof(client_addr);
	while(!isStop())
	{
		// check if there is message in queue
		if(mFifo.deQueue(msg) == 0)
		{
			InfoLog(msg<<endl);
		}
		// check if a new client connect to mcc
		len = sizeof(client_addr);
		int connfd = accept(mListenFd, (struct sockaddr*)&client_addr, &len);
		if(connfd == -1 && errno == EAGAIN)
			;
		else if(connfd == -1)
			throw "error while accept";
		else
		{
			InfoLog("new client connected");
			int flags = fcntl(connfd, F_GETFL, 0);
			fcntl(connfd, F_SETFL, flags | O_NONBLOCK); 
			mConnFdList.push_back(connfd);
			mClientInfo[connfd] = ClientInfo(client_addr);
		}
		// handle I/O event of connected client
		PollClientFd();
	}
}

