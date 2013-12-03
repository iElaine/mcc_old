/**
 * @file mcckeyboardgeneric.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include "mcckeyboardgeneric.h"
#include "mcckeyboardmanager.h"
REGISTER_KEYBOARD("generic", MCCKeyboardGeneric);

/**
 * @brief constructor
 */
MCCKeyboardGeneric::MCCKeyboardGeneric(MCCKeyboardManager *mgr) : MCCKeyboard(mgr)
{

}

/**
 * @brief destructor
 */
MCCKeyboardGeneric::~MCCKeyboardGeneric()
{

}

int MCCKeyboardGeneric::Load(const char *keyboard, const char *matrix, int baudrate, int databits, int stopbits, char parity) 
{
	return MCCKeyboard::Load(keyboard, matrix, baudrate, databits, stopbits, parity);
}

int MCCKeyboardGeneric::OnRxKeyboardPacket(SerialPort *sp)
{
	char buf[256];
	int nread = cmd_length;
	// read 1 byte until read byte equal start byte
	do {
		sp->Read(buf, 1);
		nread--;
	} while(buf[0] != cmd_start_byte && nread);
	// if no start byte read , return
	if(nread == 0)
		return 0;
	// read the rest of packet
	sp->Readn((buf+1), cmd_length - 1);
	// map cmd packet to key value of keyboard
	eMCCKeyValue key = GetKeyValue(buf, cmd_length);
	if(key == eMCCKey_Unknown)
		return 0;
	// handle keyin with a internal fsm
	int status = onKeyin(key);

	// fsm reset
	if(status == 0)
		ResetState();
	else if(status == 1)
		CacheCmd(buf, cmd_length);
	// fsm complete
	else
	{
		CacheCmd(buf, cmd_length);
		MCCKeyboardMessage &msg = BuildKeyboardMessage(1, cur_cam_id, cur_mon_id);
		NotifyManager(msg);
	}
	return 0;
}

int MCCKeyboardGeneric::OnRxMatrixPacket(SerialPort *sp)
{
	char buf[256];
	int len;
	len = sp->Read(buf, sizeof(buf));
	GetKeyboardSp()->Write(buf, len);
	return 0;
}

eMCCKeyValue MCCKeyboardGeneric::GetKeyValue(char *buf, int cmd_length) 
{ 
	return eMCCKey_Unknown;
}

void MCCKeyboardGeneric::ResetState() {
}

int MCCKeyboardGeneric::CacheCmd(char *buf, int cmd_length) {
	return 0;
}

int MCCKeyboardGeneric::onKeyin(eMCCKeyValue key) { 
	switch(m_state) {
		case eKBState_Null:
			cur_cam_id = 0;
			cur_mon_id = 0;

			break;
		case eKBState_Cam:
			break;
		case eKBState_Mon:
			break;
	}
	return 0;
}

int MCCKeyboardGeneric::NotifyManager(MCCKeyboardMessage &msg) { 

	return 0;
}

MCCKeyboardMessage &MCCKeyboardGeneric::BuildKeyboardMessage(int matrix_id, int local_cam_id, int local_mon_id) { 
	MCCKeyboardMessage *pmsg = new MCCKeyboardMessage();
//	pmsg->keyboard_id = id;
//	pmsg->camera_id = MCCGlobalID(matrix_id, local_cam_id);
//	pmsg->monitor_id = local_mon_id;
	return *pmsg;
}
