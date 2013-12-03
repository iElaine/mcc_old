#include <iostream>
#include <string.h>
#include <cstdio>
#include <iomanip>

using namespace std;

int main(int argc, char *argv[])
{
	if(argc < 2)
		return 0;
	int len = strlen(argv[1]);
	cout<<argv[1]<<" len = "<<len<<endl;
	cout<<hex;
	const char *buf = argv[1];
	for(int i=0;i<len;++i)
		cout<<hex<<setfill('0')<<setw(2)<<(int)(0xff & buf[i])<<' ';
	cout<<endl;
	cout<<dec;
	return 0;
}
