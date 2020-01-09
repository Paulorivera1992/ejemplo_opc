
CC=gcc
LINKS=-lpthread -lpython2.7


server: 
	$(CC) -std=c99 open62541.c server.c -o server $(LINKS)
    

clean:
	-rm -f server

run:
	./server
