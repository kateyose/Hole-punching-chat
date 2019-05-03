Simple command line conversation app using UDP holepunching technique.

To run this program, you need to have a server to help peers establish connection.
Before you run the program, you need to change server address and port in the udp_client.c and udp_server.c as you desire.

What you need to do in server.
$ make
$ ./server

What you need to do in local client. (You need 2 clients to have conversation)
$ make
$ ./client

This program was taken from the mwarning's github repository.
https://github.com/mwarning/UDP-hole-punching-examples.
