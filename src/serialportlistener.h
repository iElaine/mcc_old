/**
 * @file serialportlistener.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __SERIALPORTLISTENER_H__
#define __SERIALPORTLISTENER_H__
class SerialPort;
class SerialPortListener {
	public:
		/**
		 * @brief constructor
		 */
		SerialPortListener() {};
		/**
		 * @brief destructor
		 */
		virtual ~SerialPortListener() {};
	protected:
		friend class SerialPort;
		friend int SerialPortRxThread(void *param);
		virtual int on_recv(SerialPort *sp, void *data, int len) { return 0;};
		virtual int on_send(SerialPort *sp, void *data, int len) { return 0;};
		virtual int on_recv(SerialPort *sp) { return 0;};
		virtual int on_send(SerialPort *sp) { return 0;};
		virtual int on_error(int error) { return 0;};
		int m_priority;
};


#endif /* __SERIALPORTLISTENER_H__ */
