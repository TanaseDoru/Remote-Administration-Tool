CC = gcc
CFLAGS = -Wall

all: client server

server: server.c messageManager.c fileManager.c utils.c serverInterface.c
	$(CC) server.c serverManager.c  fileManager.c messageManager.c serverInterface.c utils.c -o server

client: client.o keylog.o clientManager.c utils.c messageManager.c
	$(CC) client.o keylog.o clientManager.c utils.c messageManager.c -o client

client.o: client.c keylog.h
	$(CC) $(CFLAGS) -c client.c

keylog.o: keylog.c keylog.h
	$(CC) $(CFLAGS) -c keylog.c

clean:
	rm -f *.o client
	rm keylog*.txt
