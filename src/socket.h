#ifndef __SOCKET_H__
#define __SOCKET_H__
class Socket {
	public:
		enum ProtocalType{
			TCP,
			UDP
		};
		enum IOType {
			Block,
			NonBlock
		};
		Socket(ProtocalType protocl, IOType iotype);
		virtual ~Socket();
		int Send(void *buf, int len);
		int Recv(void *buf, int len);
		int Connect(Address &addr);
	protected:
		ProtocalType mProtocalType;
		IOType mIOType;
		int fd;
};
#endif
