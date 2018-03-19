CC = gcc
CFLAGS = -g -Wall

objects = server.o client.o

all: server client

server: server.c
	$(CC) -o server server.c

client: client.c
	$(CC) -o client client.c

.PHONY : clean
clean:
	rm $(all) $(objects)
