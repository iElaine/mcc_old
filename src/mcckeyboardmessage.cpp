/**
 * @file mcckeyboardmessage.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <cstdio>
#include "mcckeyboardmessage.h"
using namespace std;

int MCCKeyboardMessage::counter = 0;
/**
 * @brief constructor
 */
MCCKeyboardMessage::MCCKeyboardMessage() : camera_id(MCCGlobalID(-1,-1))
{
	type = 0;
	keyboard_id = 0;
	monitor_id = 0;
	ptz_opt_type = STOP;
	ptz_opt_vstep = 0;
	ptz_opt_hstep = 0;
	left_up_monitor_id = 0;
	right_down_monitor_id = 0;
	seq = counter++;
	cached_cmd = NULL;
	lock_resource= 0;
}

/**
 * @brief destructor
 */
MCCKeyboardMessage::~MCCKeyboardMessage()
{

}

ostream &
MCCKeyboardMessage::encode(ostream &os) const
{
	os<<endl<<"keyboard message info ["<<this<<']'<<endl;
	if(type == eKBMsg_SWITCH)
		os<<"type\t: switch"<<endl;
	else if(type == eKBMsg_PTZ)
		os<<"type\t: ptz"<<endl;
	else
		os<<"type\t: unknown"<<endl;
	os<<"cid\t: "<<camera_id.GetMatrixID()<<','<<camera_id.GetLocalID()<<endl;
	os<<"kid\t: "<<keyboard_id<<endl;
	os<<"mid\t: "<<monitor_id<<endl;
	os<<"ptz\t: "<<ptz_opt_type<<endl;
	os<<"vstep\t: "<<ptz_opt_vstep<<endl;
	os<<"hstep\t: "<<ptz_opt_hstep<<endl;
	os<<"seq\t: "<<seq<<endl;
	return os;
}

ostream & 
operator<<(ostream &os, const MCCKeyboardMessage &msg)
{
	return msg.encode(os);
}

bool MCCKeyboardMessage::operator==(const MCCKeyboardMessage& msg) const
{
	if(type == msg.type &&
	keyboard_id == msg.keyboard_id &&
	camera_id == msg.camera_id &&
	monitor_id == msg.monitor_id &&
	left_up_monitor_id == msg.left_up_monitor_id &&
	right_down_monitor_id == msg.right_down_monitor_id &&
	ptz_opt_type == msg.ptz_opt_type &&
	ptz_opt_vstep == msg.ptz_opt_vstep &&
	ptz_opt_hstep == msg.ptz_opt_hstep &&
	lock_resource == msg.lock_resource)
		return true;
	else
		return false;

}
