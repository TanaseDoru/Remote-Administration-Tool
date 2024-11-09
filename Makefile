CC = gcc
CFLAGS = -Wall

all: client server

server: server.c
	$(CC) server.c serverManager.c clientManager.c fileManager.c -o server

client: client.o keylog.o
	$(CC) client.o keylog.o -o client

client.o: client.c keylog.h
	$(CC) $(CFLAGS) -c client.c

keylog.o: keylog.c keylog.h
	$(CC) $(CFLAGS) -c keylog.c

clean:
	rm -f *.o client
	rm keylog*.txt
