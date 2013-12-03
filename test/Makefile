CPPFLAGS=-Wall -g -I../include -I../thirdparty/include
LDFLAGS+=`pkg-config --cflags --libs libpjproject`
LDFLAGS+=-L../lib
LDFLAGS+=-ltinyxml -llog4cplus -lconfile -lgtest_main
LDFLAGS+=-Wl,-rpath=./lib
# TARGETS=recorder writesp test 

vpath %.h ../include ../thirdparty/include

test:test.cpp ../src/*.o
	g++ $(CPPFLAGS) $^ -o ../bin/$@ $(LDFLAGS) 
clean:
	rm -rf ../bin/test
