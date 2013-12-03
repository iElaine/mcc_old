#include "module.h"
#include <iostream>
using namespace std;

class HahaModule : public Module {
	virtual void action() { cout<<"Haha"<<endl;}
};
DECL_MODULE(HahaModule);

