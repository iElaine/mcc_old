#include "serverprocess.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <stdexcept>
#include <errno.h>
#include <string.h>
#include "log.h"

void
ServerProcess::Daemonize()
{
	pid_t pid;
	if((pid =  fork()) < 0)
	{
		throw std::runtime_error(strerror(errno));
	}
	else if(pid != 0)
	{
		exit(0);
	}
	if(chdir("/") < 0)
		throw std::runtime_error(strerror(errno));
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}
