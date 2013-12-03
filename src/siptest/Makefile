CFLAGS=-Wall -g
LDFLAGS+=`pkg-config --cflags --libs libpjproject` -lpthread

sip: sip.c
	gcc $(CFLAGS) sip.c -o sip $(LDFLAGS)

clean:
	rm -rf sip
