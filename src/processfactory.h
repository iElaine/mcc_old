/**
 * @file processfactory.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __PROCESSFACTORY_H__
#define __PROCESSFACTORY_H__
#include <string.h>
#include "socketpair.h"

typedef int (*ProcessFunc)(void *param, int len, SocketPair *sock);

class ProcessFactory;

class ProcessInfo {
	public:
		SocketPair &GetSocketPair() { return *m_sock;}
		virtual ~ProcessInfo() { delete m_sock;}
	protected:
		friend class ProcessFactory;
		ProcessInfo(const char *name, int pid, SocketPair *sock, ProcessFunc func, void *param, int len);
		int m_pid;
		char m_name[256];
		SocketPair *m_sock;
		ProcessFunc m_func;
		void *m_param;
		int m_len;
};

class ProcessFactory {
	public:
		/**
		 * @brief constructor
		 */
		ProcessFactory();
		/**
		 * @brief destructor
		 */
		virtual ~ProcessFactory();
		static ProcessInfo *CreateProcess(const char *name, ProcessFunc func, void *param, int len, ProcessFunc onkill = NULL); 
		static ProcessInfo *ForkProcess(const char *name, const char *argv[]) { return NULL;}
		static int KillProcess(ProcessInfo *info);
	protected:
		static bool bInstallSigHandler;
};
#endif /* __PROCESSFACTORY_H__ */
