/**
 * @file mcckeyboardcmdrecord.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCKEYBOARDCMDRECORD_H__
#define __MCCKEYBOARDCMDRECORD_H__

#include "mcckeyboard.h"
#include <cstdio>
#include <string.h>
#define EXT_NAME ".rec"

enum eKBCmdValue {
	eKBValue_0 = 0,
	eKBValue_1,
	eKBValue_2,
	eKBValue_3,
	eKBValue_4,
	eKBValue_5,
	eKBValue_6,
	eKBValue_7,
	eKBValue_8,
	eKBValue_9,
};

class MCCKeyboardCmdRecordSet {
	public:
		MCCKeyboardCmdRecordSet() {};
		MCCKeyboardCmdRecordSet(const char *name, int baudrate, int databits, int stopbits, char parity);
		int AddRecord(void *cmd, int len, eKBCmdValue value);
		void Init(const char *name, int baudrate, int databits, int stopbits, char parity);
		int DumpToFile(const char *filename = NULL);
	protected:
		struct Record{
		};
		char m_name[256];
		int m_baudrate;
		int m_databits;
		int m_stopbits;
		char m_parity;
		std::vector<MCCKeyboardCmdRecord> m_records;
};


class MCCKeyboardCmdRecord {
	public:
		/**
		 * @brief constructor
		 */
		MCCKeyboardCmdRecord();
		MCCKeyboardCmdRecord(void *p_cmd, int length, eKBCmdValue kvalue);
		/**
		 * @brief destructor
		 */
		virtual ~MCCKeyboardCmdRecord();
		void *cmd;
		int len;
		eKBCmdValue value;
};
#endif /* __MCCKEYBOARDCMDRECORD_H__ */
