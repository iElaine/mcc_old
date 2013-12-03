all: mcc
	
mcc:
	cd src && make

test: mcc
	cd test && make
	cd bin && ./test

clean:
	cd src && make clean
	cd test && make clean
