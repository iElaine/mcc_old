#ifndef __MODULE_H__
#define __MODULE_H__
#define DECL_MODULE(classname) \
	classname _module;
class Module {
	public:
		virtual void action() = 0;
};
#endif
