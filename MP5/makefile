# makefile

all: dataserver simpleclient

NetworkRequestChannel.o: NetworkRequestChannel.H NetworkRequestChannel.C
	g++ -std=c++11 -c -lpthread -g NetworkRequestChannel.C

dataserver: dataserver.C NetworkRequestChannel.o 
	g++ -g -o dataserver dataserver.C NetworkRequestChannel.o -lpthread

simpleclient: simpleclient.C NetworkRequestChannel.o boundedBuffer.C semaphore.C
	g++ -std=c++11 -lpthread -g -o simpleclient simpleclient.C NetworkRequestChannel.o boundedBuffer.C semaphore.C

clean:
	rm -rf *.o fifo* dataserver simpleclient
