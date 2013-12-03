/**
 * @file mccclientmanager.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include "mccclientmanager.h"
#include "mcckeyboardmanager.h"
#include "mcckeyboardmessage.h"
#include "testclient/mccclient.h"
#include "log.h"

using namespace mcc;
using namespace std;

/**
 * @brief constructor
 */
MCCClientManager::MCCClientManager(MCCKeyboardManager &mgr) :
	mMgr(mgr)
{

}

/**
 * @brief destructor
 */
MCCClientManager::~MCCClientManager()
{

}

int MCCClientManager::Start(const char *ip, unsigned short port)
{
	mServer = new AsyncTCPServer(this);
	return mServer->Start(ip, port);
}

void MCCClientManager::onNewConnection(int fd)
{
	set<int>::iterator i = mConnFdList.find(fd);
	if(i == mConnFdList.end())
		mConnFdList.insert(fd);
}

void MCCClientManager::onDisconnect(int fd)
{
	set<int>::iterator i = mConnFdList.find(fd);
	if(i != mConnFdList.end())
		mConnFdList.erase(i);
}

void MCCClientManager::onExcept(int fd, int err)
{
}

void MCCClientManager::onReadable(AsyncTCPServer &server, int fd, void *data, size_t len)
{
	client_packet packet;
	char *buf = (char *)data;
	while(len >= sizeof(packet))
	{
		memcpy(&packet, buf, sizeof(packet));
		MCCKeyboardMessage msg;
		switch(packet.type)
		{
			case 1:
				msg.type = packet.type;
				msg.keyboard_id = packet.bind_kb;
				msg.camera_id = MCCGlobalID(packet.matrix_id, packet.cid);
				msg.monitor_id = packet.mid;
				mMgr.Post(msg);
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
				mMgr.Post(msg);
				break;
			case 3:
				msg.type = packet.type;
				msg.keyboard_id = packet.bind_kb;
				msg.camera_id = MCCGlobalID(packet.matrix_id, packet.cid);
				//////////
				msg.monitor_id = packet.lumid;
				msg.left_up_monitor_id = packet.lumid;
				msg.right_down_monitor_id = packet.rdmid;
				mMgr.Post(msg);
				break;
			case 4:
			default:
				break;
		}
		len -= sizeof(packet);
		buf += sizeof(packet);
	}
}

void MCCClientManager::onWriteble(AsyncTCPServer &server, int fd)
{
}

void
MCCClientManager::Notify(MCCKeyboardMessage &msg)
{
	set<int>::iterator i;
	static client_packet last_packet;
	client_packet packet;
	int len;
	packet.type = msg.type;
	packet.matrix_id = msg.camera_id.GetMatrixID();
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
		InfoLog("same ptz command, ignored");
		return;
	}
	last_packet = packet;
	for(i = mConnFdList.begin(); i!=mConnFdList.end();++i)
	{
		len = send(*i, &packet, sizeof(client_packet), 0);
		InfoLog("notify client to "<<*i<<msg);
		if((len < 0 && errno == ECONNRESET) || len == 0)
		{
			close(*i);
			mConnFdList.erase(i);
		}
	}
}


