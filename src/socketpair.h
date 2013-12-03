/**
 * @file socketpair.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __SOCKETPAIR_H__
#define __SOCKETPAIR_H__

class SocketPair {
	public:
		/**
		 * @brief constructor
		 */
		SocketPair();
		/**
		 * @brief destructor
		 */
		virtual ~SocketPair();

		/**
		 * called after fork()
		 */
		int Init(int pid);
		int Send(char *msg, int len);
		int Recv(char *msg, int len);
		int Close();
		const int GetHandle() const { return owned_fd;};
	protected:
		int fd[2];
		int owned_fd;
		int status;
};
#endif /* __SOCKETPAIR_H__ */
