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
#include "mcckeyboardrc.h"
#include "log.h"

using namespace std;

REGISTER_KEYBOARD("rc", MCCKeyboardRC);


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
MCCKeyboardRC::MCCKeyboardRC(MCCKeyboardManager *mgr) : MCCKeyboard(mgr)
{
}

/**
 * @brief destructor
 */
MCCKeyboardRC::~MCCKeyboardRC()
{

}

int MCCKeyboardRC::Send(void *buf, int len, int channel) {
	char channel_buf[8][1024];
	memcpy(channel_buf[channel], buf, len);
	return 0;
}

int 
MCCKeyboardRC::buildMessageFromPacket(char *packet, int len, MCCKeyboardMessage &msg)
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
MCCKeyboardRC::buildPacketFromMessage(char *buf, int len, MCCKeyboardMessage &msg)
{
	if(msg.type == eKBMsg_SWITCH)
	{
		unsigned char begin, end, row, col;
		begin = 0; end = 0;
		col = msg.monitor_id;
		row = 1;
		while(col > 6)
		{
			col -= 6;
			row++;
		}
		begin = (col & 0x0f) << 4;
		begin &= 0xf0;
		begin |= (row & 0x0f);
		end = begin;
		buf[0] = 0xd5;
		buf[1] = begin;
		buf[2] = end;
		buf[3] = 0x20;
		if(msg.camera_id.GetLocalID() > 8)
			buf[4] = 0x00;
		else 
			buf[4] = 0x02;
		buf[5] = 0xaa;
		char *cmd = buf + 6;
		if(msg.camera_id.GetLocalID() > 8)
			sprintf(cmd, "PA01SW%02d%02dNT", msg.camera_id.GetLocalID() - 8, msg.monitor_id);
		else
			sprintf(cmd, "PV01SW%02d%02dNT", msg.camera_id.GetLocalID(), msg.monitor_id);
		char *p = strstr(cmd, "T");
		return p - buf+ 1;
	}
	else if(msg.type == eKBMsg_Pin)
	{
		unsigned char begin, end, row, col, row1, col1;
		int rows, cols;
		begin = 0; end = 0;
		col = msg.left_up_monitor_id;
		row = 1;
		while(col > 6)
		{
			col -= 6;
			row++;
		}
		begin = (col & 0x0f) << 4;
		begin &= 0xf0;
		begin |= (row & 0x0f);
		col1 = msg.right_down_monitor_id;
		row1 = 1;
		while(col1 > 6)
		{
			col1 -= 6;
			row1++;
		}
		end = (col1 & 0x0f) << 4;
		end &= 0xf0;
		end |= (row1 & 0x0f);
		if(row1 < row || col1 < col)
			return 0;
		rows = row1 - row + 1;
		cols = col1 - col + 1;
		buf[0] = 0xd5;
		buf[1] = begin;
		buf[2] = end;
		buf[3] = 0x20;
		if(msg.camera_id.GetLocalID() > 8)
			buf[4] = 0x00;
		else
			buf[4] = 0x02;
		buf[5] = 0xaa;
		char *p = buf + 6;
		InfoLog(rows<<","<<cols);
		for(int i = 0;i<rows;++i)
		{
			for(int j=0;j<cols;++j)
			{
				if(msg.camera_id.GetLocalID() > 8)
					sprintf(p, "PA01SW%02d%02dNT", msg.camera_id.GetLocalID() - 8, msg.left_up_monitor_id + j + 6*i);
				else
					sprintf(p, "PV01SW%02d%02dNT", msg.camera_id.GetLocalID(), msg.left_up_monitor_id + j + 6*i);
				p += 12;
			}
		}
		return p - buf;
	}
	return 0;
}

	int 
MCCKeyboardRC::Load(const char *keyboard, 
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
	mQueryThread->Start();
	m_matrix2 = new SerialPort();
	if(m_matrix2 == NULL)
		return -1;
	if(m_matrix2->Init() != 0)
		return -1;
	if(m_matrix2->RegisterListener(this) != 0)
		return -1;
	rc = m_matrix2->Open("/dev/ttyUSB8", baudrate, databits, stopbits, parity);
	return rc;
}	

int
MCCKeyboardRC::Unload()
{
	MCCKeyboard::Unload();
	if(mQueryThread.get())
	{
		mQueryThread->Stop();
	}
	return 0;
}

bool 
MCCKeyboardRC::ValidatePacket(char *packet, int len)
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

int MCCKeyboardRC::OnRxKeyboardPacket(SerialPort *sp) {
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

int MCCKeyboardRC::OnRxMatrixPacket(SerialPort *sp) {
	char queries = 0;
	sp->Read(&queries, 5);
	return 0;
}

int 
MCCKeyboardRC::SendSwitch(int local_cid, int local_mid)
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
MCCKeyboardRC::SendSwitch(MCCKeyboardMessage &msg)
{
	MCCKeyboardVirtual *kb = dynamic_cast<MCCKeyboardVirtual *>(mMgr->getKeyboard(1));
	if(kb)
		kb->Update(msg);
	char buf[1024];
	int len = 0;
	if((len = buildPacketFromMessage(buf, 1024, msg)) > 0)
	{
		for(int i = 0;i<6;++i)
		{
			m_matrix->Write(buf+i, 1);
			m_matrix2->Write(buf+i, 1);
			usleep(10000);
		}
		char *p = buf + 6;
		len -= 6;
		usleep(100000);
		while(len > 0)
		{
			for(int i = 0;i<12;++i)
			{
				m_matrix->Write(p+i, 1);
				m_matrix2->Write(p+i, 1);
				usleep(10000);
			}
			usleep(100000);
			p += 12;
			len -= 12;
		}
		usleep(100000);
	}
	cout<<hex;
	for(int i = 0; i< 6;++i)
		cout<<hex<<setfill('0')<<setw(2)<<(int)(0xff & buf[i])<<' ';
	cout<<endl<<dec;
	cout.flush();
	InfoLog("cz08 switch"<<len<<"  "<<(const char *)(buf+6));
	return 0;
//	return mFifo.inQueue(msg);
}

int 
MCCKeyboardRC::SendPtz(int local_cid)
{
	return 0;
}

int 
MCCKeyboardRC::SendPtz(MCCKeyboardMessage &msg)
{
	MCCKeyboardVirtual *kb = dynamic_cast<MCCKeyboardVirtual *>(mMgr->getKeyboard(1));
	if(kb)
		kb->Update(msg);
	return 0;
//	return mFifo.inQueue(msg);
}
int 
MCCKeyboardRC::Notify(MCCKeyboardMessage &msg, int status_code, const char *reason) 
{ 
	InfoLog(status_code<<','<<reason);
	return 0;
}
