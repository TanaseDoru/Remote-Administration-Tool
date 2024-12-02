CC = gcc
CFLAGS = -Wall -g

all: client server

server: server.c serverManager.o fileManager.c utils.c serverInterface.c
	$(CC) server.c serverManager.o  fileManager.c messageManager.c serverInterface.c utils.c -o server

client: client.o keylog.o clientManager.c utils.c messageManager.c
	$(CC) client.o keylog.o clientManager.c utils.c messageManager.c -o client

client.o: client.c keylog.h
	$(CC) $(CFLAGS) -c client.c

keylog.o: keylog.c keylog.h
	$(CC) $(CFLAGS) -c keylog.c

serverManager.o: serverManager.c serverManager.h
	$(CC) $(CFLAGS) -c serverManager.c


clean:
	rm -f *.o client
	rm -f keylog*.txt
	rm -f Tanase*
	rm -f tudor*
