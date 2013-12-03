#include <iostream>
#include <string.h>
#include "serialport.h"
using namespace std;

int main(int argc, char *argv[])
{
	const char default_path[] = "/dev/ttyUSB2";
	const char *path;
	if(argc < 2)
		path = default_path;
	else
		path = argv[1];
	char buf[1024];
	SerialPort sp;
	sp.Init();
	sp.Open(path, 9600, 8, 1, 'n');
	while(1)
	{
		cout<<"Please key in data"<<endl;
		cin>>buf;
		sp.Write(buf, strlen(buf)+1);
	}
	return 0;
}
	
