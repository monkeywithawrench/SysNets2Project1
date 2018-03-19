CC = gcc
CFLAGS = -g -Wall

objects = server.o client.o

all: server client

server: server.c
	$(CC) -o server server.c $(CFLAGS)

client: client.c
	$(CC) -o client client.c $(CFLAGS)

.PHONY : clean
clean:
	rm $(all) $(objects)
