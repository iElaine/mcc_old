#include <iostream>
#include "module.h"
using namespace std;


class MyModule : public Module {
	public:
		virtual void action() { cout<<"Hello world"<<endl;}
};
DECL_MODULE(MyModule);

