/**
 * @file mcckeyboardgeneric.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCKEYBOARDGENERIC_H__
#define __MCCKEYBOARDGENERIC_H__
#include <map>
#include "mcckeyboard.h"
#include "mcckeyvalue.h"
//#include "mccKeyboardmessage.h"
class MCCKeyboardManager;
class MCCKeyboardGeneric : public MCCKeyboard {
	public:
		/**
		 * @brief constructor
		 */
		MCCKeyboardGeneric(MCCKeyboardManager *mgr);
		/**
		 * @brief destructor
		 */
		virtual ~MCCKeyboardGeneric();
		int Load(const char *keyboard, const char *matrix, int baudrate, int databits, int stopbits, char parity);
	protected:
		virtual int OnRxKeyboardPacket(SerialPort *sp);
		virtual int OnRxMatrixPacket(SerialPort *sp);
		eMCCKeyValue GetKeyValue(char *buf, int cmd_length);
		void ResetState();
		int CacheCmd(char *buf, int cmd_length);
		int onKeyin(eMCCKeyValue key);
		int NotifyManager(MCCKeyboardMessage &msg);
		MCCKeyboardMessage &BuildKeyboardMessage(int matrix_id, int local_cam_id, int local_mon_id); 
		enum eKBState {
			eKBState_Null = 0,
			eKBState_Cam,
			eKBState_Mon,
		};
		int				cmd_length;
		char			cmd_start_byte;
		int				m_state;
		int				cur_cam_id;
		int				cur_mon_id;
		std::map<int, eMCCKeyValue> keyvalue_map;
		// hash_table for cmd <---> keyvalue
		// hash_table for msg <---> cached_cmd 
};
#endif /* __MCCKEYBOARDGENERIC_H__ */
