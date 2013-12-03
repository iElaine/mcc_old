/**
 * @file socketpair.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "socketpair.h"

enum eSocketPair_Status {
	eSocketPair_Loaded = 0,
	eSocketPair_Inited,
	eSocketPair_Closed,
};

/**
 * @brief constructor
 */
SocketPair::SocketPair()
{
	if(socketpair(AF_LOCAL, SOCK_STREAM, 0, fd) == -1)
		throw std::string("fail to create socket pair");
	owned_fd = -1;
	status = eSocketPair_Loaded;
}

/**
 * @brief destructor
 */
SocketPair::~SocketPair()
{
	if(status != eSocketPair_Closed)
		Close();
}

int SocketPair::Init(int pid) {
	if(status != eSocketPair_Loaded)
		return -1;
	if(pid == 0)
	{
		close(fd[0]);
		owned_fd = fd[1];
	}
	else
	{
		close(fd[1]);
		owned_fd = fd[0];
	}
	status = eSocketPair_Inited;
	return 0;
}

int SocketPair::Send(char *msg, int len) {
	if(status != eSocketPair_Inited)
		return -1;
	return send(owned_fd, msg, len, 0);
}

int SocketPair::Recv(char *msg, int len) {
	if(status != eSocketPair_Inited)
		return -1;
	return recv(owned_fd, msg, len, 0);
}

int SocketPair::Close()
{
	if(status == eSocketPair_Inited)
	{
		close(owned_fd);
	}
	else if(status == eSocketPair_Loaded)
	{
		close(fd[0]);
		close(fd[1]);
	}
	status = eSocketPair_Closed;
	return 0;
}
