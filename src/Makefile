objects=threadif.o \
		serialport.o \
		mcckeyboard.o \
		socketpair.o \
		processfactory.o \
		mcckeyboardmanager.o \
		mcckeyboardmessage.o \
		mcckeyboardjxj.o \
		mcckeyboardjxjen.o \
		mcckeyboardcz08.o \
		mcckeyboardrc.o \
		mcckeyboardgeneric.o \
		mcckeyboardvirtual.o \
		mccglobalid.o \
		mccuser.o \
		mccresourcemanager.o \
		mccsystem.o \
		mccrunner.o \
		configparser.o \
		semaphore.o \
		asynctcpserver.o \
		asynctcpclient.o \
		serverprocess.o \
		mccagentserver.o \
		mccagentclient.o \
		mccagentmessage.o \
		mccclientmanager.o \
		httpserver.o \
		util.o \

CPPFLAGS=-Wall -g -I../include -I../thirdparty/include
LDFLAGS+=`pkg-config --cflags --libs libpjproject`
LDFLAGS+=-L../lib 
LDFLAGS+=-ltinyxml -llog4cplus -lconfile
LDFLAGS+=-Wl,-rpath=./lib
TARGETS=mcc 
GTEST_ROOT=./gtest-1.6.0/

.PHONY:all clean again

vpath %.h ../thirdparty/include

all:$(TARGETS)

test:
	cd test && make

mccsys:mccsys.cpp $(objects) confile.o
	g++ $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

mcc:mcc.cpp $(objects)
	g++ $(CPPFLAGS) $^ -o ../bin/$@ $(LDFLAGS)

writesp: writesp.cpp $(objects) confile.o
	g++ $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

recorder: recorder.cpp $(objects) confile.o
	g++ $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

$(objects):%.o:%.cpp %.h
	g++ $(CPPFLAGS) -c $< -o $@ 

sip: sip.c
	gcc -g -Wall sip.c -o sip $(LDFLAGS)

.PHONY:install
install:all
	cp mcc mcc.ini /mccsystem

clean:
	rm -rf $(objects) ../bin/mcc
