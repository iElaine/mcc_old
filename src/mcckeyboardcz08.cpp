/**
 * @file mcckeyboardjxj.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <cstdio>
#include <cstring>
#include <assert.h>
#include "autolock.h"
#include "mcckeyboardvirtual.h"
//#include "mcckeyboardmanager.h"
#include "mcckeyboardcz08.h"
#include "log.h"

using namespace std;

REGISTER_KEYBOARD("cz08", MCCKeyboardCZ08);

static Mutex __mLockWrite;

/*
 * this thread is used to send query command to simulate a matrix
 * so that this keyboard module can receive command from keyboard
 */
class QueryThread : public ThreadIf {
	public:
		QueryThread(const char *name, SerialPort &keyboard) : ThreadIf(name), mKeyboard(keyboard) {}
	protected:
		virtual void thread()
		{
			char query = 0x50;
			while(!isStop())	
			{
				usleep(300000);
//				AutoLock lock(&__mLockWrite);
				mKeyboard.Write(&query, 1);
			}
		}
		SerialPort &mKeyboard;
};

/**
 * @brief constructor
 */
MCCKeyboardCZ08::MCCKeyboardCZ08(MCCKeyboardManager *mgr) : MCCKeyboard(mgr)
{
}

/**
 * @brief destructor
 */
MCCKeyboardCZ08::~MCCKeyboardCZ08()
{

}

int MCCKeyboardCZ08::Send(void *buf, int len, int channel) {
	char channel_buf[8][1024];
	memcpy(channel_buf[channel], buf, len);
	return 0;
}

int 
MCCKeyboardCZ08::buildMessageFromPacket(char *packet, int len, MCCKeyboardMessage &msg)
{
	msg.keyboard_id = mId;
	msg.camera_id = MCCGlobalID(mMgr->GetMatrixId(), (int)packet[4]);
	if(packet[2] == 0x11)
	{
		msg.type = eKBMsg_SWITCH;
		msg.monitor_id = (int)packet[5];
	}
	else if(packet[2] == 0x21)
	{
		msg.type = eKBMsg_PTZ;
		if(!(packet[3] || packet[5] || packet[6]))
		{
			msg.ptz_opt_type = MCCKeyboardMessage::STOP;
		}
		// if there is v opt
		msg.ptz_opt_hstep = (packet[5] & 0x7f);
		// if there is h opt
		msg.ptz_opt_vstep = (packet[6] & 0x7f); 
		// check opt type
		if((packet[3] & 0xff) == 0x40)
			msg.ptz_opt_type |= MCCKeyboardMessage::ZOOMIN;
		else if((packet[3] & 0xff) == 0x80)
			msg.ptz_opt_type |= MCCKeyboardMessage::ZOOMOUT;
		if(msg.ptz_opt_hstep)
		{
			if((packet[5] & 0x80) == 0)
				msg.ptz_opt_type |= MCCKeyboardMessage::LEFT;
			else 
				msg.ptz_opt_type |= MCCKeyboardMessage::RIGHT;
		}
		if(msg.ptz_opt_vstep)
		{
			if((packet[6] & 0x80) == 0)
				msg.ptz_opt_type |= MCCKeyboardMessage::UP;
			else 
				msg.ptz_opt_type |= MCCKeyboardMessage::DOWN;
		}
	}
	else
		msg.type = eKBMsg_UNKNOWN;
	return 0;
}

	int 
MCCKeyboardCZ08::buildPacketFromMessage(char *buf, int len, MCCKeyboardMessage &msg)
{
	if(msg.type == eKBMsg_SWITCH)
	{
		sprintf(buf, "<swit,%d,0,%d>", msg.monitor_id, msg.camera_id.GetLocalID());
		char *p = strstr(buf, ">");
		return p - buf+ 1;
	}
	return 0;
}

	int 
MCCKeyboardCZ08::Load(const char *keyboard, 
		const char *matrix, 
		int baudrate, 
		int databits, 
		int stopbits, 
		char parity)
{
	int rc;
	if((rc = MCCKeyboard::Load(keyboard, matrix, baudrate, databits, stopbits, parity)) != 0)
		return rc;
	ThreadIf *thread = new QueryThread("Query Thread", *(this->m_keyboard));
	if(thread == NULL)
	{
		Unload();
		return -1;
	}
	mQueryThread.reset(thread);
	return mQueryThread->Start();
}	

int
MCCKeyboardCZ08::Unload()
{
	MCCKeyboard::Unload();
	if(mQueryThread.get())
	{
		mQueryThread->Stop();
	}
	return 0;
}

bool 
MCCKeyboardCZ08::ValidatePacket(char *packet, int len)
{
	if(packet[0] != 0xff)
		return false;
	if(len != 8)
		return false;
	char sum = 0;
	for(int i=1;i<6;i++)
		sum += packet[i];
	return sum == packet[7];
}

int MCCKeyboardCZ08::OnRxKeyboardPacket(SerialPort *sp) {
	static unsigned char confirm[] = {0xf2, 0x00, 0x01, 0x00, 0x40, 0x00, 0x00, 0x41, 0xff};
	char buf[1024];
	// read command data from keyboard
	do {
		sp->Read(buf, 1);
	} while((0xff & buf[0]) != 0xff);
	sp->Readn(buf+1, 7);
	cout<<"rx:";
	for(int i=0;i<8;i++)
	{
		cout<<hex<<setfill('0')<<setw(2)<<(int)(0xff & buf[i])<<' ';
	}
	cout<<endl;
	cout<<dec;
	if(buf[2] == 0x41)
	{
		InfoLog("receive register");
		sp->Write(confirm, sizeof(confirm));
	}
	MCCKeyboardMessage msg;
	buildMessageFromPacket(buf, 8, msg);
	if(!isLocked())
	{
		mMgr->Post(msg);
	}
//	InfoLog(msg);
	return 0;
}

int MCCKeyboardCZ08::OnRxMatrixPacket(SerialPort *sp) {
	char queries = 0;
	sp->Read(&queries, 5);
	return 0;
}

int 
MCCKeyboardCZ08::SendSwitch(int local_cid, int local_mid)
{
	char cmd[8];
	cmd[0] = 0xff;
	cmd[1] = 0x00;
	cmd[2] = 0x11;
	cmd[3] = 0x00;
	cmd[4] = (char)(0xff & local_cid);
	cmd[5] = (char)(0xff & local_mid);
	cmd[6] = 0x55;
	cmd[7] = 0x66 + cmd[4] + cmd[5];
	m_matrix->Write(cmd, 8);
	return 0;
}

int 
MCCKeyboardCZ08::SendSwitch(MCCKeyboardMessage &msg)
{
	MCCKeyboardVirtual *kb = dynamic_cast<MCCKeyboardVirtual *>(mMgr->getKeyboard(1));
	if(kb)
		kb->Update(msg);
	char buf[20];
	int len = 0;
	if((len = buildPacketFromMessage(buf, 20, msg)) > 0)
		m_matrix->Write(buf, len);
	InfoLog("cz08 switch"<<buf);
	return 0;
//	return mFifo.inQueue(msg);
}

int 
MCCKeyboardCZ08::SendPtz(int local_cid)
{
	return 0;
}

int 
MCCKeyboardCZ08::SendPtz(MCCKeyboardMessage &msg)
{
	MCCKeyboardVirtual *kb = dynamic_cast<MCCKeyboardVirtual *>(mMgr->getKeyboard(1));
	if(kb)
		kb->Update(msg);
	return 0;
//	return mFifo.inQueue(msg);
}
int 
MCCKeyboardCZ08::Notify(MCCKeyboardMessage &msg, int status_code, const char *reason) 
{ 
	InfoLog(status_code<<','<<reason);
	return 0;
}
