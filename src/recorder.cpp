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

class KeyboardCmdRecordSet {
	public:
		KeyboardCmdRecordSet() {};
		KeyboardCmdRecordSet(const char *name, int baudrate, int databits, int stopbits, char parity);
		int AddRecord(void *cmd, int len, eKBCmdValue value);
		void Init(const char *name, int baudrate, int databits, int stopbits, char parity);
		int DumpToFile(const char *filename = NULL);
	protected:
		struct Record{
			void *cmd;
			int len;
			eKBCmdValue value;
			Record(void *p_cmd, int length, eKBCmdValue kvalue) {
				memcpy(cmd, p_cmd, len);
				len = length;
				value = kvalue;
			};
		};
		char m_name[256];
		int m_baudrate;
		int m_databits;
		int m_stopbits;
		char m_parity;
		std::vector<Record > m_records;
};

KeyboardCmdRecordSet::KeyboardCmdRecordSet(const char *name, int baudrate, int databits, int stopbits, char parity)
{
	Init(name, baudrate, databits, stopbits, parity);
}

void KeyboardCmdRecordSet::Init(const char *name, int baudrate, int databits, int stopbits, char parity) {
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

int KeyboardCmdRecordSet::DumpToFile(const char *filename) 
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
	std::vector<Record>::iterator itr = m_records.begin();
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

int KeyboardCmdRecordSet::AddRecord(void *cmd, int len, eKBCmdValue value) {
	m_records.push_back(Record(cmd, len, value));
	return 0;
}

class KeyboardCmdRecorder : public MCCKeyboard {
	public:
		KeyboardCmdRecorder() {};
		virtual ~KeyboardCmdRecorder() {};
		virtual int Load(const char *kbname, const char *kbpath, const char *mtpath,int baudrate, int databits, int stopbits, char parity) {
			recordset.Init(kbname, baudrate, databits, stopbits, parity);
			return MCCKeyboard::Load(kbpath, mtpath, baudrate, databits, stopbits, parity);
		}
		int ReadAndAddRecord(eKBCmdValue value) {
			char buf[1024];
//			sleep(1);
			int len = m_keyboard->Read(buf, sizeof(buf));
			if(len > 0)
				return recordset.AddRecord(buf, len, value);
			return -1;
		}
		KeyboardCmdRecordSet &GetRecordSet() { return recordset;}
		int SaveRecords(const char *filename = NULL) {
			return recordset.DumpToFile(filename);
		}
	protected:
		KeyboardCmdRecordSet recordset;
};

int main(int argc, char *argv[]) 
{
	// setup keyboard : name, baudrate, databits, stopbits, parity
	if(argc < 2)
	{
		printf("Usage: recorder TOTAL_RECORDS_NUMBER\n eg. recorder 10\n");
		return 0;
	}
	int total = 0;
	char name[100];
	int baudrate, databits, stopbits;
	char parity;
	sscanf(argv[1], "%d", &total);
	KeyboardCmdRecorder recorder;
	printf("Input keyboard name\t:");
	int len;
	len = scanf("%s",name);
	printf("Input keyboard baudrate\t:");
	len = scanf("%d",&baudrate);
	printf("Input keyboard databits\t:");
	len = scanf("%d",&databits);
	printf("Input keyboard stopbits\t:");
	len = scanf("%d",&stopbits);
	printf("Input keyboard parity\t:");
	getchar();
	len = scanf("%c",&parity);
	len = len;
	recorder.Load(name, "/dev/ttyUSB0", "/dev/ttyUSB1", baudrate, databits, stopbits, parity);
	for(int i=0;i<total;i++)
	{
		printf("please press key %d, if it's done press enter to continue!\n", i);
		getchar();
		recorder.ReadAndAddRecord((eKBCmdValue)i);
	}
	recorder.SaveRecords();
	return 0;
}
