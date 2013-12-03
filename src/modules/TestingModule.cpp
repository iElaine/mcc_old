#include "module.h"
#include <iostream>
using namespace std;
class TestingModule : public Module {
	public:
		virtual void action() { cout<<"testing"<<endl;}
};
DECL_MODULE(TestingModule);

