Modules=MyModule.so HahaModule.so TestingModule.so

all: $(Modules) main

$(Modules):%.so:%.cpp module.h
	g++ -shared -fPIC -o $@ $<

main: main.cpp module.h
	g++ $< -o $@ -ldl

clean:
	rm *.so main -rf
