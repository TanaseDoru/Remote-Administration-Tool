CC = gcc
CFLAGS = -Wall -g

all: client server

server: server.c serverManager.o utils.c serverInterface.c constraints.h
	$(CC) server.c serverManager.o messageManager.c serverInterface.c utils.c -o server

client: client.o keylog.o clientManager.c utils.c messageManager.c
	$(CC) client.o keylog.o clientManager.c utils.c messageManager.c -o client

client.o: client.c keylog.h
	$(CC) $(CFLAGS) -c client.c

keylog.o: keylog.c keylog.h
	$(CC) $(CFLAGS) -c keylog.c

serverManager.o: serverManager.c serverManager.h
	$(CC) $(CFLAGS) -c serverManager.c

kill:
	sudo kill -9 $(pidof client)

clean:
	rm -f *.o client
	rm -f keylog*.txt
	rm -rf Tanase*
	rm -rf tudor*

