#include "serialport.h"

class SPListener : public SerialPortListener {
	protected:
		virtual int on_recv(SerialPort *sp) {
			char buf[100];
			int len = sp->Read(buf, sizeof(buf));
		}
};
