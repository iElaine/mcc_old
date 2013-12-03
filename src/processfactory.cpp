/**
 * @file processfactory.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "processfactory.h"

bool ProcessFactory::bInstallSigHandler = false;

ProcessInfo::ProcessInfo(const char *name, int pid, SocketPair *sock, ProcessFunc func, void *param, int len) : 
	m_pid(pid),
	m_sock(sock),
	m_func(func),
	m_param(param),
	m_len(len) 
{
	memset(m_name, 0, sizeof(m_name));
	strcpy(m_name, name);
}

static void _child_sig_handler(int signo)
{
	pid_t pid;
	int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		printf("Child Process %d exit\n", pid);
	}
}

static void _kill_sig_handler(int signo)
{
}

/**
 * @brief constructor
 */
ProcessFactory::ProcessFactory()
{
}

/**
 * @brief destructor
 */
ProcessFactory::~ProcessFactory()
{

}

ProcessInfo *ProcessFactory::CreateProcess(const char *name, ProcessFunc func, void *param, int len, ProcessFunc onkill) 
{
	int pid;
	if(name == NULL || func == NULL)
		return NULL;
	SocketPair *sock = new SocketPair();
	pid = fork();
	if(pid == -1)
		return NULL;
	sock->Init(pid);
	if(pid != 0)
	{
		if(bInstallSigHandler == false)
		{
			if(signal(SIGCHLD, _child_sig_handler) == 0)
			{
				printf("install signal handler for SIGCHLD!\n");
				bInstallSigHandler = true;
			}
		}
		printf("process fork:%d --> %d\n", getpid(), pid);
		return new ProcessInfo(name, pid, sock, func, param, len);
	}
	else
	{
		int res;
		if(signal(SIGKILL, _kill_sig_handler) == 0)
			printf("install signal handler for SIGKILL");
		res = func(param, len, sock);
		exit(res);
	}
	return NULL;
}

int ProcessFactory::KillProcess(ProcessInfo *info) 
{
	return 0;
}
