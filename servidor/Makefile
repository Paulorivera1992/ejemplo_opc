
CC=gcc
LINKS=open62541/build/open62541.c


server: 
	$(CC) -std=c99 $(LINKS) server.c -o server -lpthread 
    

clean:
	-rm -f server

run:
	./server
