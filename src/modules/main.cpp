#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include "module.h"

using namespace std;

int main(int argc, char* argv[])
{
	void *handles[3];
	const char modname[3][100] = {"./MyModule.so", "./HahaModule.so", "./TestingModule.so"};
	Module* modules[3];
	int i;
	for(i=0;i<3;++i)
	{
		handles[i] = dlopen(modname[i], RTLD_LAZY | RTLD_NODELETE);
		if(!handles[i])
		{
			cout<<dlerror()<<endl;
			return 0;
		}
		modules[i]= (Module *)dlsym(handles[i], "_module");
		modules[i]->action();
	}
}
