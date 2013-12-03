#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <termio.h>
#include "serialport.h"
#include "log.h"
#include "mutex.h"
#include "autolock.h"
#include "thread.h"
#include "serialportlistener.h"

//=================================
// MACROs
#define INVALID_FD				(-1)
#define	SP_THREAD_STACK_SIZE	(4096)

using namespace std;

/**
 * @file SerialPort.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
static int _serial_baudrate_to_flag(int speed)
{
	static int baudflag_arr[] = {
		B921600, B460800, B230400, B115200, B57600, B38400,
		B19200, B9600, B4800, B2400, B1800, B1200,
		B600, B300, B150, B110, B75, B50
	};
	static int speed_arr[] = {
		921600, 460800, 230400, 115200, 57600, 38400,
		19200, 9600, 4800, 2400, 1800, 1200,
		600, 300, 150, 110, 75, 50
	};
	unsigned int i;
	for (i = 0; i < sizeof(speed_arr)/sizeof(int); i++) {
		if (speed == speed_arr[i]) {
			return baudflag_arr[i];
		}
	}
    return B9600;
}

int SerialPortRxThread(void *param)
{
	SerialPort *sp = (SerialPort *)param;
	fd_set rset;
	struct timeval tv;
	int rc;
	InfoLog( "SerialPort:"<<sp->m_file<<": rx_thread start");
	tcflush(sp->m_fd,TCIOFLUSH);   
	while(1)
	{
		{
			AutoLock lock(sp->m_lock);
			if(sp->m_status == MCC_SP_ECLOSING)
			{
				sp->m_status = MCC_SP_ECLOSED;
				break;
			}
		}
		// IO operations & notify listener
		FD_ZERO(&rset);
		FD_SET(sp->m_fd, &rset);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		rc = select((sp->m_fd + 1) , &rset, NULL, NULL, &tv);
		if(rc == 0)
//			InfoLog( "SerialPort:"<<sp->m_file<<": read timeout");
			rc = rc;
		else if(FD_ISSET(sp->m_fd, &rset))
		{
			if(sp->m_listener_list.size() > 0)
			{
				vector<SerialPortListener *>::iterator itr = sp->m_listener_list.begin();
				while(itr != sp->m_listener_list.end())
				{
					rc = (*itr)->on_recv(sp);
					itr++;
				}
			}
		}
		else
		{
			InfoLog( "SerialPort:"<<sp->m_file<<": select() error close port");
			if(sp->m_listener_list.size() > 0)
			{
				vector<SerialPortListener *>::iterator itr = sp->m_listener_list.begin();
				while(itr != sp->m_listener_list.end())
				{
					rc = (*itr)->on_error(0);
					itr++;
				}
			}
	
		}
	}
	InfoLog(  "SerialPort:"<<sp->m_file<<": rx_thread exit");
	return 0;
}

//=================================
// public function implementations
/**
 * @brief constructor
 */
SerialPort::SerialPort()
{
	m_setup.baudrate = 9600;
	m_setup.databits = 8;
	m_setup.stopbits = 1;
	m_setup.parity = 'n';
	m_setup.iotype = O_RDWR;
	m_status = MCC_SP_ECLOSED;
	m_fd = INVALID_FD;
	memset(m_file, 0, sizeof(m_file));
	strcpy(m_file, "/dev/ttyS0");
	m_lock = new Mutex();
}

/**
 * @brief destructor
 */
SerialPort::~SerialPort()
{
	Close();
}

int SerialPort::Init()
{
//	m_lock = new Mutex();
//	return m_lock->Init();
	return 0;
}

int SerialPort::Setup(int baud, int databits, int stopbits, int parity)
{
	static struct termio oterm_attr;
    struct termio term_attr;
    /* Get current setting */
    if (ioctl(m_fd, TCGETA, &term_attr) < 0) {
        return -1;
    }
    /* Backup old setting */
    memcpy(&oterm_attr, &term_attr, sizeof(struct termio));
    term_attr.c_iflag &= ~(INLCR | IGNCR | ICRNL | ISTRIP | IXON | BRKINT | INPCK);
    term_attr.c_oflag &= ~(OPOST | ONLCR | OCRNL);
    term_attr.c_lflag &= ~(ISIG | ECHO | ICANON | NOFLSH);
    term_attr.c_cflag &= ~CBAUD;
    term_attr.c_cflag |= CLOCAL | CREAD | _serial_baudrate_to_flag(baud);
    /* Set databits */
    term_attr.c_cflag &= ~(CSIZE);
    switch (databits) {
        case 5:
            term_attr.c_cflag |= CS5;
            break;
        case 6:
            term_attr.c_cflag |= CS6;
            break;
        case 7:
            term_attr.c_cflag |= CS7;
            break;
        case 8:
        default:
            term_attr.c_cflag |= CS8;
            break;
    }
    /* Set parity */
    switch (parity) {
        case 1: /* Odd parity */
            term_attr.c_cflag |= (PARENB | PARODD);
            break;
        case 2: /* Even parity */
            term_attr.c_cflag |= PARENB;
            term_attr.c_cflag &= ~(PARODD);
            break;
        case 0: /* None parity */
        default:
            term_attr.c_cflag &= ~(PARENB);
            break;
    }
    /* Set stopbits */
    switch (stopbits) {
        case 2: /* 2 stopbits */
            term_attr.c_cflag |= CSTOPB;
            break;

        case 1: /* 1 stopbits */
        default:
            term_attr.c_cflag &= ~CSTOPB;
            break;
    }
    term_attr.c_cc[VMIN] = 1;
    term_attr.c_cc[VTIME] = 0;
    if (ioctl(m_fd, TCSETAW, &term_attr) < 0) {
        return -1;
    }
    if (ioctl(m_fd, TCFLSH, 2) < 0) {
        return -1;
    }
    return 0;
}

int SerialPort::Open(const char *file, int baudrate, int databits, int stopbits, char parity)
{
	/* check if port is already opened */
	AutoLock lock(m_lock);
	int rc;
	if(m_status != MCC_SP_ECLOSED)
		return 0;

	/* open serial port */
	m_fd = open(file, m_setup.iotype, 0);
	if(m_fd < 0)
	{
		m_fd = INVALID_FD;
		InfoLog(  "SerialPort:"<<file<<": open failed");
		return m_fd;
	}
	/* execute m_setup */
	rc = Setup(baudrate, 
			databits, 
			stopbits, 
			parity);
	if(rc < 0)
	{
		close(m_fd);
		m_fd = INVALID_FD;
		InfoLog(  "SerialPort:"<<file<<": setup failed ");
		return rc;
	}
	/* create thread for receive bytes */
	m_rxthread = new Thread;
	rc = m_rxthread->Start(SerialPortRxThread, this);
	if(rc != 0)
	{
		
		close(m_fd);
		m_fd = INVALID_FD;
		delete m_rxthread;
		m_rxthread = NULL;
		InfoLog(  "SerialPort:"<<file<<": fail to create rx thread");
		return rc;
	}
	/* update status & m_setup infomation */
	m_status = MCC_SP_EOPEN;	
	m_setup.baudrate = baudrate;
	m_setup.databits = databits;
	m_setup.stopbits = stopbits;
	m_setup.parity   = parity;
	strcpy(m_file, file);
	InfoLog(   "SerialPort:"<<file<<": open successfully with configuration "<< \
				m_setup.baudrate<<','<< \
				m_setup.databits<<','<< \
				m_setup.stopbits<<','<< \
				m_setup.parity);
	return 0;
}

int SerialPort::Reopen()
{
	return Open(m_file, m_setup.baudrate, m_setup.databits, m_setup.stopbits, m_setup.parity);
}

bool SerialPort::isOpen()
{
	return (m_status == MCC_SP_EOPEN);
}

int SerialPort::GetBaudrate()
{
	return m_setup.baudrate;
}

int SerialPort::GetDatabits()
{
	return m_setup.databits;
}

int SerialPort::GetStopbits()
{
	return m_setup.stopbits;
}

char SerialPort::GetParity()
{
	return m_setup.parity;
}

int SerialPort::Close()
{
	int rc;
	{
		AutoLock lock(m_lock);
		if(m_status == MCC_SP_ECLOSED || m_status == MCC_SP_ECLOSING)
		{
			return 0;
		}
		rc = close(m_fd);
		if(rc != 0)
			return rc;
		m_fd = INVALID_FD;
		m_status = MCC_SP_ECLOSING;
	}
	InfoLog(  "SerialPort:"<<m_file<<": closing");
	m_rxthread->Join();
	delete m_rxthread;
	InfoLog(  "SerialPort:"<<m_file<<": closed");
	return 0;
}

int SerialPort::UnregisterListener(SerialPortListener* listener)
{
	AutoLock lock(m_lock);
	//m_listener_list.erase(listener);
	InfoLog(  "SerialPort:"<<m_file<<": clear listener "<<listener);
	return 0;
}

int SerialPort::RegisterListener(SerialPortListener *listener)
{
	AutoLock lock(m_lock);
	m_listener_list.push_back(listener);
	InfoLog(  "SerialPort:"<<m_file<<": set listener "<<listener);
	return 0;
}

int SerialPort::Read(void *buf, int len)
{
	if(m_status != MCC_SP_EOPEN)
		return -1;
	return read(m_fd, buf, len);
}

int SerialPort::Readn(void *buf, int len)
{
	if(m_status != MCC_SP_EOPEN)
		return -1;
	int total = len;
	int n_read = 0;
	int left = len;
	char *buff = (char *)buf;
	while(left > 0)
	{
		n_read += Read(buff+n_read, left);
		left = total - n_read;
	}
	return total;
}

int SerialPort::Write(void *buf, int len)
{
	if(m_status != MCC_SP_EOPEN)
		return -1;
	return write(m_fd, buf, len);
}

void SerialPort::Dump()
{
	const char status_str[3][20] = {
		{"closed"},
		{"open"},
		{"closing"}
	};
	InfoLog(  "device   :\t"<<m_file);
	InfoLog(  "baudrate :\t"<<m_setup.baudrate);
	InfoLog(  "databits :\t"<<m_setup.databits);
	InfoLog(  "stopbits :\t"<<m_setup.stopbits);
	InfoLog(  "parity   :\t"<<m_setup.parity);
	InfoLog(  "status   :\t"<<status_str[m_status]);
}

