/**
 * @file SerialPort.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__
#include <vector>


typedef enum ESerialPortStatus {
	MCC_SP_ECLOSED = 0,
	MCC_SP_EOPEN,
	MCC_SP_ECLOSING,
} ESerialPortStatus;

class Mutex;
class Thread;
class SerialPortListener;

class SerialPort {
	public:
		/**
		 * @brief constructor
		 */
		SerialPort();
		/**
		 * @brief destructor
		 */
		virtual ~SerialPort();
		int Open(const char *file, int baudrate, int databits, int stopbits, char parity);
		int Close();
		int Init();
		int Read(void *buf, int len);
		int Readn(void *buf, int len);
		int Write(void *buf, int len);
		int RegisterListener(SerialPortListener *listener);
		int UnregisterListener(SerialPortListener *listener);
		int Reopen();
		bool isOpen();
		int GetBaudrate();
		int GetDatabits();
		int GetStopbits();
		char GetParity();
		void Dump();
	protected:
		friend int SerialPortRxThread(void *);
		int Setup(int baudrate, int databits, int stopbits, int parity);
		struct {
			int baudrate;
			int databits;
			int stopbits;
			char parity;
			int iotype;
		} m_setup;
		char m_file[256];
		int m_fd;
		int m_status;
		Mutex *m_lock;
		Thread *m_rxthread;
		std::vector<SerialPortListener*> m_listener_list;
};

#endif /* __SERIALPORT_H__ */
