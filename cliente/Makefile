
CC=gcc
LINKS=-lpthread


server: 
	$(CC) -std=c99 open62541.c cliente.c -o cliente $(LINKS)
    

clean:
	-rm -f cliente

run:
	./cliente
