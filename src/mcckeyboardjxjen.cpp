/**
 * @file mcckeyboardjxj.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <cstdio>
#include <cstring>
#include <assert.h>
#include "autolock.h"
#include "mcckeyboardmanager.h"
#include "mcckeyboardjxjen.h"
#include "log.h"

using namespace std;

REGISTER_KEYBOARD("jxj_en", MCCKeyboardJXJEn);

int move(char *buf, int type, int inport, int left_up_monitor_id, int right_down_monitor_id);
int create(char *buf, int type, int inport, int left_up_monitor_id, int right_down_monitor_id);
int close(char *buf, int type, int inport);
static Mutex __mLockWrite;

/*
 * this thread is used to send query command to simulate a matrix
 * so that this keyboard module can receive command from keyboard
 */
class JXJEnQueryThread : public ThreadIf {
	public:
		JXJEnQueryThread(const char *name, SerialPort &keyboard, SerialPort &matrix) : ThreadIf(name), mKeyboard(keyboard), mMatrix(matrix) {}
	protected:
		virtual void thread()
		{
			char query = 0x50;
			while(!isStop())	
			{
				char buf[] = "NewReadCh 1\r\n";
				usleep(300000);
//				AutoLock lock(&__mLockWrite);
				mKeyboard.Write(&query, 1);
				mMatrix.Write(buf, sizeof(buf) - 1);
			}
		}
		SerialPort &mKeyboard;
		SerialPort &mMatrix;
};

/**
 * @brief constructor
 */
MCCKeyboardJXJEn::MCCKeyboardJXJEn(MCCKeyboardManager *mgr) : MCCKeyboard(mgr)
{
}

/**
 * @brief destructor
 */
MCCKeyboardJXJEn::~MCCKeyboardJXJEn()
{

}

int MCCKeyboardJXJEn::Send(void *buf, int len, int channel) {
	char channel_buf[8][1024];
	memcpy(channel_buf[channel], buf, len);
	return 0;
}

int 
MCCKeyboardJXJEn::buildMessageFromPacket(char *packet, int len, MCCKeyboardMessage &msg)
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
MCCKeyboardJXJEn::buildPacketFromMessage(char *buf, int len, MCCKeyboardMessage &msg)
{
	buf[0] = 0xff;
	buf[1] = 0x00;
	if(msg.type == eKBMsg_SWITCH)
	{
		buf[2] = 0x11;
		buf[3] = 0x00;
		buf[4] = (char)msg.camera_id.GetLocalID();
		buf[5] = (char)msg.monitor_id;
		buf[6] = 0x55;
	}
	else if(msg.type == eKBMsg_PTZ)
	{
		// cmd
		buf[2] = 0x21;
		// zoom param
		if(msg.ptz_opt_type & MCCKeyboardMessage::ZOOMIN)
			buf[3] = 0x40;
		if(msg.ptz_opt_type & MCCKeyboardMessage::ZOOMOUT)
			buf[3] = 0x80;
		// camera id
		buf[4] = (char)msg.camera_id.GetLocalID();
		// v operation
		if(msg.ptz_opt_type & MCCKeyboardMessage::LEFT)
			buf[5] = msg.ptz_opt_hstep;
		if(msg.ptz_opt_type & MCCKeyboardMessage::RIGHT)
			buf[5] = 0x80 + msg.ptz_opt_hstep;
		// h operation
		if(msg.ptz_opt_type & MCCKeyboardMessage::UP)
			buf[6] = msg.ptz_opt_vstep;
		if(msg.ptz_opt_type & MCCKeyboardMessage::DOWN)
			buf[6] = 0x80 + msg.ptz_opt_vstep;
		if(msg.ptz_opt_type == MCCKeyboardMessage::STOP)
		{
			buf[3] = 0x00;
			buf[5] = 0x00;
			buf[6] = 0x00;
		}
	}
	buf[7] = buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6];
	return 0;
}

	int 
MCCKeyboardJXJEn::Load(const char *keyboard, 
		const char *matrix, 
		int baudrate, 
		int databits, 
		int stopbits, 
		char parity)
{
	int rc;
	if((rc = MCCKeyboard::Load(keyboard, matrix, baudrate, databits, stopbits, parity)) != 0)
		return rc;
	m_matrix2 = new SerialPort();
	if(m_matrix2 == NULL)
		return -1;
	if(m_matrix2->Init() != 0)
		return -1;
	rc = m_matrix2->Open("/dev/ttyUSB7", 115200, 8, 1, 'n');
	ThreadIf *thread = new JXJEnQueryThread("Query Thread", *(this->m_keyboard), *(this->m_matrix2));
	if(thread == NULL)
	{
		Unload();
		return -1;
	}
	mQueryThread.reset(thread);
	mQueryThread->Start();
	return rc;
}	

int
MCCKeyboardJXJEn::Unload()
{
	MCCKeyboard::Unload();
	if(mQueryThread.get())
	{
		mQueryThread->Stop();
	}
	return 0;
}

bool 
MCCKeyboardJXJEn::ValidatePacket(char *packet, int len)
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

int MCCKeyboardJXJEn::OnRxKeyboardPacket(SerialPort *sp) {
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
	//if(!ValidatePacket(buf, 8))
	//	return 0;
	// if is register then confirm
	if(buf[2] == 0x41)
	{
		InfoLog("receive register");
//		AutoLock lock(&__mLockWrite);
		sp->Write(confirm, sizeof(confirm));
	}
//	if(buf[2] == 0x21)
//	{
//		m_matrix->Write(buf, 8);
//		return 0;
//	}
	// SendSwitch((int)buf[4], (int)buf[5]);
	// build keyboard message from packet;
	MCCKeyboardMessage msg;
	buildMessageFromPacket(buf, 8, msg);
	if(msg == mLastMessage)
		return 0;
	mLastMessage = msg;
	mMgr->Post(msg);
//	InfoLog(msg);
	return 0;
}

int MCCKeyboardJXJEn::OnRxMatrixPacket(SerialPort *sp) {
	char queries = 0;
	static char buf[8] = {0xff, 0x00, 0x11, 0x00, 0x01, 0x01, 0x55, 0x68};
	char reg[] = {0xff, 0x00, 0x41, 0x11, 0x55, 0x00, 0x00, 0xa7};
	static int rcv_cnt = 0;
	static MCCKeyboardMessage msg;
	int len = sp->Read(&queries, 1);
	assert(len == 1);
	if(rcv_cnt == 0)
	{
		if((queries & 0xff) != (0x50 + mId))
			return 0;
		assert(sp->Write(reg, 8) == 8);

		rcv_cnt++;
	}
	else if((0xff & queries) == (0x50 + mId))
	{
		if(mFifo.deQueue(msg) == 0)
		{
//			InfoLog(msg);
			buildPacketFromMessage(buf, 8, msg);
			cout<<"tx:";
			for(int i=0;i<8;i++)
			{
				cout<<hex<<setfill('0')<<setw(2)<<(int)(0xff & buf[i])<<' ';
			}
			cout<<endl;
			cout<<dec;
		}
		sp->Write(buf, 8);
	}
	return 0;
}

int 
MCCKeyboardJXJEn::SendSwitch(int local_cid, int local_mid)
{
	return 0;
}

int 
MCCKeyboardJXJEn::SendSwitch(MCCKeyboardMessage &msg)
{
	char buf[300];
	int len;
	if(msg.left_up_monitor_id == 0 && msg.right_down_monitor_id == 0)
	{
		msg.left_up_monitor_id = msg.monitor_id;
		msg.right_down_monitor_id = msg.monitor_id;
	}
	if(msg.camera_id.GetLocalID() <= 4)
	{
		InfoLog("msg:"<<msg);
		msg.type = eKBMsg_SWITCH;
		if(msg.left_up_monitor_id > 4)
			len = create(buf, 0, 3, msg.left_up_monitor_id, msg.right_down_monitor_id);
		else
			len = create(buf, 0, msg.left_up_monitor_id - 1, msg.left_up_monitor_id, msg.right_down_monitor_id);
		mFifo.inQueue(msg);
	}
	else
	{
		len = create(buf, 1, msg.camera_id.GetLocalID()-5, msg.left_up_monitor_id, msg.right_down_monitor_id);
	}
	InfoLog("Send:"<<buf);
	m_matrix2->Write(buf, len);
	return 0;
}

int 
MCCKeyboardJXJEn::SendPtz(int local_cid)
{
	return 0;
}

int 
MCCKeyboardJXJEn::SendPtz(MCCKeyboardMessage &msg)
{
	return mFifo.inQueue(msg);
}

int 
MCCKeyboardJXJEn::Notify(MCCKeyboardMessage &msg, int status_code, const char *reason) 
{ 
	InfoLog(status_code<<','<<reason);
	return 0;
}

enum _SourceType {
	VIDEO = 0,
	DVI = 1,
	HDMI = 2
};
#define ROWS 3
#define COLS 3
void getEdge(int top_left, int bottom_right, int &left, int& right, int& top, int& bottom)
{
	left = (top_left - 1)%3;
	right = (bottom_right - 1)%3;
	top = (top_left - 1)/3;
	bottom = (bottom_right - 1)/3;	
}

int move(char *buf, int type, int inport, int left_up_monitor_id, int right_down_monitor_id)
{
	int len;
	int left,right;
	int top,bottom;
	getEdge(left_up_monitor_id, right_down_monitor_id, left, right, top, bottom);
	switch (type)
	{
		case VIDEO:
			sprintf(buf,"VWinSize %d %d %d %d %d\r\n",inport, left*1366, top*768, right*1366+1366, bottom*768 + 768);
			len = strlen(buf);
			break;
		case DVI:
			sprintf(buf,"DWinSize %d %d %d %d %d\r\n",inport, left*1366, top*768, right*1366 + 1366, bottom*768 + 768);
			len = strlen(buf);
			break;
		case HDMI:
			len = 0;
			break;
		default:
			len = 0;
			break;
	}
	return len;		
}

int create(char *buf, int type, int inport, int left_up_monitor_id, int right_down_monitor_id)
{
	return move(buf, type, inport, left_up_monitor_id, right_down_monitor_id);
}

int close(char *buf, int type, int inport)
{
	int len;
	switch (type)
	{
		case VIDEO:
			sprintf(buf,"VWinSwitch %d\r\n",inport);
			len = strlen(buf);
			break;
		case DVI:
			sprintf(buf,"DWinSwitch %d\r\n",inport);
			len = strlen(buf);
			break;
		case HDMI:
			len = 0;
			break;
		default:
			len = 0;
			break;			
	}
	return len;	
}

