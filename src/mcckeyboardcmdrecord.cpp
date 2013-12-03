/**
 * @file mcckeyboardcmdrecord.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include "mcckeyboardcmdrecord.h"

/**
 * @brief constructor
 */
MCCKeyboardCmdRecord::MCCKeyboardCmdRecord()
{

}

MCCKeyboardCmdRecord::MCCKeyboardCmdRecord(void *p_cmd, int length, eKBCmdValue kvalue) 
{
	memcpy(cmd, p_cmd, len);
	len = length;
	value = kvalue;
}
/**
 * @brief destructor
 */
MCCKeyboardCmdRecord::~MCCKeyboardCmdRecord()
{

}

MCCKeyboardCmdRecordSet::MCCKeyboardCmdRecordSet(const char *name, int baudrate, int databits, int stopbits, char parity)
{
	Init(name, baudrate, databits, stopbits, parity);
}

void MCCKeyboardCmdRecordSet::Init(const char *name, int baudrate, int databits, int stopbits, char parity) {
	m_baudrate = baudrate;
	m_databits = databits;
	m_stopbits = stopbits;
	m_parity = parity; 
	strcpy(m_name, name);
}	

char *GetCmdValueName(eKBCmdValue value)
{
	static char hello[] = "hello";
	return hello;
}

int MCCKeyboardCmdRecordSet::DumpToFile(const char *filename) 
{
	char namebuf[1024];
	if(filename == NULL)
		filename = m_name;
	sprintf(namebuf,"%s%s", filename, EXT_NAME);
	FILE *fp = fopen(namebuf, "w");
	if(fp == NULL)
		return -1;
	fprintf(fp, "Keyboard Name:%s\n"
			"Configuration:%d,%d,%d,%c\n"
			"Total Records:%d\n",
			m_name, m_baudrate, m_databits, m_stopbits, m_parity, m_records.size());
	std::vector<MCCKeyboardCmdRecord>::iterator itr = m_records.begin();
	fprintf(fp,"#[KeyName, CommandLength, CommandData]\n");
	while(itr != m_records.end())
	{
		fprintf(fp,"[ %s, %d, ", ::GetCmdValueName(itr->value), itr->len);
		char *data = (char *)itr->cmd;
		for(int i=0;i<itr->len;i++)
			fprintf(fp, "0x%02x ", data[i]);
		fprintf(fp,"]\n");
		itr++;
	}
	fclose(fp);
	return 0;
}

int MCCKeyboardCmdRecordSet::AddRecord(void *cmd, int len, eKBCmdValue value) {
	m_records.push_back(MCCKeyboardCmdRecord(cmd, len, value));
	return 0;
}
