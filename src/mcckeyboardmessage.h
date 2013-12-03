/**
 * @file mcckeyboardmessage.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCKEYBOARDMESSAGE_H__
#define __MCCKEYBOARDMESSAGE_H__

#include "mccglobalid.h"
#include <iostream>

enum eKBMsgType {
	eKBMsg_UNKNOWN = 0,
	eKBMsg_SWITCH,
	eKBMsg_PTZ,
	eKBMsg_Pin
};
//class MCCKeyboardMessage;
//class MCCKeyboardMessageHelper {
//	public:
//		static MCCKeyboardMessage &BuildSwitchMessage(int kb_id, int cam_id, int mon_id, int seq) {
//			MCCKeyboardMessage *msg = new MCCKeyboardMessage();
//			msg->type = eKBM
//		}
//};
class MCCKeyboardMessage {
	public:
		/**
		 * @brief constructor
		 */
		MCCKeyboardMessage();
		/**
		 * @brief destructor
		 */
		virtual ~MCCKeyboardMessage();
		enum PTZType {
			UP		= 0x0001,
			DOWN	= 0x0002,
			LEFT	= 0x0004,
			RIGHT	= 0x0008,
			ZOOMIN	= 0x0010,
			ZOOMOUT = 0x0020,
			STOP	= 0
		};
		int				type;
		int				keyboard_id;
		MCCGlobalID		camera_id;
		int				monitor_id;
		int				left_up_monitor_id;
		int				right_down_monitor_id;
		int				ptz_opt_type;
		int				ptz_opt_vstep;
		int				ptz_opt_hstep;
		int				seq;
		int				lock_resource;  // 0-ignore 1-lock 2-unlock
		void		   *cached_cmd;
		std::ostream & encode(std::ostream& os) const;
		bool			operator==(const MCCKeyboardMessage& msg) const;
	private:
		static int counter;
};

std::ostream & operator<<(std::ostream &os, const MCCKeyboardMessage &msg);

class MCCKeyboardMessageHelper {
	public:
		static bool buildSwitchMessage(int matrixid, int inport, int outport, MCCKeyboardMessage& msg) {
			msg.type = eKBMsg_SWITCH;
			msg.keyboard_id = 0;
			msg.camera_id = MCCGlobalID(matrixid, inport);
			msg.monitor_id = outport;
			return true;
		}
};
#endif /* __MCCKEYBOARDMESSAGE_H__ */
