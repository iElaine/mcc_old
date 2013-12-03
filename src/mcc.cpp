#include "mccrunner.h"
#include "mcckeyboardmessage.h"
#include "mcckeyboardmanager.h"
#include "log.h"
#include <iostream>
#include <signal.h>
#include <cstdlib>
#include <assert.h>

using namespace std;

static bool finished = false;
static void
signalHandler(int signo)
{
	static int cnt = 0;
	if(cnt == 0)
	{
		InfoLog("MCC system is going to shutdown ...");
		finished = true;
	}
	else
	{
		InfoLog("MCC system shutdown right now!");
		exit(0);
	}
	cnt++;
}

int main(int argc, char *argv[])
{
	// install signal handler
#ifndef _WIN32
	if ( signal( SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		cerr << "Couldn't install signal handler for SIGPIPE" << endl;
		exit(-1);
	}
#endif

	if ( signal( SIGINT, signalHandler ) == SIG_ERR )
	{
		cerr << "Couldn't install signal handler for SIGINT" << endl;
		exit( -1 );
	}

	if ( signal( SIGTERM, signalHandler ) == SIG_ERR )
	{
		cerr << "Couldn't install signal handler for SIGTERM" << endl;
		exit( -1 );
	}
//	ConfigParser config(argc, argv, "mcc.config");
	MCCRunner mcc;
	if(mcc.run(argc, argv, "mcc.ini") != 0)
	{
		mcc.shutdown();
		return -1;
	}
	while(!finished)
	{
//		int cid, mid;
//		MCCKeyboard &kb = mcc.KeyboardManager().GetKeyboard(0);
//		cout<<"camera id :";
//		cin>>cid;
//		cout<<"monitor id:";
//		cin>>mid;
//		kb.SendSwitch(cid, mid);
//		MCCKeyboardMessage msg;
//		msg.keyboard_id = 0;
//		msg.type = eKBMsg_SWITCH;
//		msg.camera_id = MCCGlobalID(0, cid);
//		msg.monitor_id = mid;
//		mcc.KeyboardManager().Post(msg);
		usleep(1000000);
	}
	mcc.shutdown();
	return 0;
}
