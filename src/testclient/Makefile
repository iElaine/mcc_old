
all: testclient

testclient:testclient.cpp mccclient.o
	g++ -Wall -g $^ -o $@ -lpthread

server:server.c
	g++ -Wall -g server.c -o server -lpthread

client:client.c mccclient.o
	g++ -Wall -g client.c -o client -lpthread

mccclient.o: mccclient.cpp mccclient.h
	g++ -Wall -g -c $< -o $@ 

clean:
	rm -rf client 
