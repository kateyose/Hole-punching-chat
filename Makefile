CC = gcc

all : client server

client : udp_client.c
	$(CC) -o client udp_client.c -lpthread

server : udp_server.c
	$(CC) -o server udp_server.c

clean :
	rm client server
