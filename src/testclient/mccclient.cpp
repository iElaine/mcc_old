#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#if defined(WIN32)

#include <windows.h>
#include <winsock.h>
#include <winbase.h>
#include <process.h>
#define close closesocket
#pragma comment( lib, "ws2_32.lib" )

#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#define INVALID_SOCKET -1

#endif

#include "mccclient.h"

namespace mcc {

	MCCClient::MCCClient() :
mConnected(false),
	mBindedId(0),
	mFd(-1),
	mListener(NULL),
	mThread(0)
{
}

MCCClient::~MCCClient() 
{
	disConnect();
}

#ifdef WIN32
unsigned int __stdcall rx_thread(void *param)
#else
void *rx_thread(void *param)
#endif
{
	assert(param);
	MCCClient &client = *(MCCClient *)param;
	char buf[1024];
	fd_set fdset;
	struct timeval tv = {1, 0}; // 10ms
	while(client.isConnected())
	{
		FD_ZERO(&fdset);
		FD_SET(client.mFd, &fdset);
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		int rc = select(client.mFd + 1, &fdset, NULL, NULL, &tv);
		if(rc == 0)
		{
			printf("timeout\n");
			continue;
		}
		else if(rc == -1)
		{
			// IO error close connection
			client.disConnect();
			return 0;
		}
		else
		{
			recv(client.mFd, buf, sizeof(buf), 0);
			if(client.mListener)
			{
				(client.mListener)->onSwitch();
			}
		}
	}
	return 0;
}

bool
	MCCClient::Connect(const char *ip, int port, int binded_keyboard_id)
{
#ifdef WIN32
	WSADATA WSAData;
	if(WSAStartup(MAKEWORD(1,1), &WSAData))
	{
		WSACleanup();
		return false;
	}
#endif

	mFd = socket(AF_INET, SOCK_STREAM, 0);
	if(mFd < 0)
	{
		perror("socket");
#ifdef WIN32
		WSACleanup();
#endif
		return false;
	}
	printf("socket ok >> ");
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip);
	if(connect(mFd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		close(mFd);
		mFd = -1;
		perror("connect");
#ifdef WIN32
		WSACleanup();
#endif
		return false;
	}
	printf("connect ok >> ");
	mConnected = true;	
	mBindedId = binded_keyboard_id;
#ifdef WIN32
	mThread = (HANDLE)_beginthreadex(NULL, 0, rx_thread, this, 0, NULL);
	if(mThread == 0)
	{
		close(mFd);
		return false;
	}
#else
	int rc = pthread_create((pthread_t *)&mThread, NULL, rx_thread, this);
	if(rc < 0)
	{
		close(mFd);
		return false;
	}
#endif
	return true;
}

void 
	MCCClient::disConnect()
{
	if(mConnected)
	{
		close(mFd);
#ifdef WIN32
		WSACleanup();
#endif
		mFd = -1;
	}
}

int 
	MCCClient::Switch(int matrix_id, int camera_id, int monitor_id, MCCResult & result)
{
	if(!mConnected)
		return -1;
	client_packet packet;
	packet.type = 1;
	packet.matrix_id = matrix_id;
	packet.bind_kb = mBindedId;
	packet.cid = camera_id;
	packet.mid = monitor_id;
	packet.ptz_type = 0;
	packet.ptz_start = 0;
	if(send(mFd, (char *)&packet, sizeof(packet), 0) <= 0)
	{
		disConnect();
		return -1;
	}
	//	if(recv(mFd, &result, sizeof(result), 0) <= 0)
	//	{
	//		disConnect();
	//		return -1;
	//	}
	return 0;
}

int MCCClient::Switch(int matrix_id, int camera_id, int left_up_monitor_id, int right_down_monitor_id, MCCResult &)
{
	if(!mConnected)
		return -1;
	client_packet packet;
	packet.type = 3;
	packet.matrix_id = matrix_id;
	packet.bind_kb = mBindedId;
	packet.cid = camera_id;
	packet.lumid = left_up_monitor_id;
	packet.rdmid = right_down_monitor_id;
	packet.ptz_type = 0;
	packet.ptz_start = 0;
	if(send(mFd, (char *)&packet, sizeof(packet), 0) <= 0)
	{
		disConnect();
		return -1;
	}
	//	if(recv(mFd, &result, sizeof(result), 0) <= 0)
	//	{
	//		disConnect();
	//		return -1;
	//	}
	return 0;
}
int 
	MCCClient::Ptz(int type, int matrix_id, int camera_id, bool bstart, MCCResult &result)
{
	if(!mConnected)
		return -1;
	client_packet packet;
	packet.type = 2;
	packet.matrix_id = matrix_id;
	packet.bind_kb = mBindedId;
	packet.cid = camera_id;
	packet.mid = 0;
	packet.ptz_type = type;
	packet.ptz_start = bstart ? 1 : 0;
	if(send(mFd, (char *)&packet, sizeof(packet), 0) <= 0)
	{
		disConnect();
		return -1;
	}
	//	if(recv(mFd, &result, sizeof(result), 0) <= 0)
	//	{
	//		disConnect();
	//		return -1;
	//	}
	return 0;
}
}; /* namespace mcc */
