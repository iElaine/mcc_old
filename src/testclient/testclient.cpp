#include "mccclient.h"
#include <iostream>
#include <cstdio>

using namespace mcc;
using namespace std;

int main(int argc, char *argv[])
{
	MCCClient client;
	int cid, mid;
	if(argc < 2)
	{
		cid = 1;
		mid = 1;
	}
	else if(argc == 3)
	{
		sscanf(argv[1],"%d", &cid);
		sscanf(argv[2],"%d", &mid);
	}
	else
		return 0;
	if(!client.Connect("192.168.1.252", 10000, 0))
	{
		cout<<"failed to connect"<<endl;
		return 0;
	}
	MCCResult res;
	if(client.Switch(0, cid, mid, res) == 0)
		cout<<res.status_code<<endl;
	cout<<"switch camera "<<cid<<" to monitor "<<mid<<endl;
	sleep(1);
	client.disConnect();
	return 0;
}
