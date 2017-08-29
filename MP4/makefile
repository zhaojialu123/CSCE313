# makefile

all: dataserver simpleclient

reqchannel.o: reqchannel.H reqchannel.C
	g++ -c -g reqchannel.C

dataserver: dataserver.C reqchannel.o 
	g++ -g -o dataserver dataserver.C reqchannel.o -lpthread

simpleclient: simpleclient.C reqchannel.o boundedBuffer.C semaphore.C
	g++ -std=c++11 -lrt -pthread -g -o simpleclient simpleclient.C reqchannel.o boundedBuffer.C semaphore.C

clean:
	rm -rf *.o fifo* dataserver simpleclient
