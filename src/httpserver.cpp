#include "httpserver.h"
#include "util.h"
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
int 
HttpServer::Start(int port)
{
	mServer = new AsyncTCPServer(this);
	return mServer->Start(NULL, port);
}

void
HttpServer::onNewConnection(int fd)
{
	if(!mConnections.count(fd))
		mConnections.insert(fd);
}

void
HttpServer::onDisconnect(int fd)
{
	set<int>::iterator i = mConnections.find(fd);
	if(i != mConnections.end())
		mConnections.erase(i);
}

void
HttpServer::onReadable(AsyncTCPServer &server, int fd, void
*data, size_t len)
{
	string received((const char *)data);
	cout<<received<<endl;
	stringstream buf;
	string body = 
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
		"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
		"<html xmlns=\"http://www.w3.org/1999/xhtml\">"
		"<head>"
		"<meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\" />"
		"<title>Test Server</title>"
		"</head>"
		"<body bgcolor=\"#ffffff\">"
		"<p>This is test Server</p>"
		"</body>"
		"</html>";
	buf<<"HTTP/1.0 200 OK\r\n"
		"Mime-version: 1.0\r\n"
		"Pragma: no-cache\r\n"
		"Content-Length: "<<body.size()<<"\r\n"
		"Content-Type: text/html\r\n"
		"\r\n"
		<<body;
	string reply = buf.str();
	cout<<reply<<endl;
	send(fd, reply.c_str(), reply.size(), 0);
}

void
HttpServer::onWriteble(AsyncTCPServer &server, int fd)
{
}

void
HttpServer::onExcept(int fd,int err)
{
}

