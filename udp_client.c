#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFLEN 512
#define NPACK 10
#define PORT 6868

#define SRV_IP "18.225.5.21"
static int s;
struct client {
  int host;
  short port;
};

void diep(char *s) {
  perror(s);
  exit(1);
}

void *send_message_to_peer(void *si_other) {
  char buffer[BUFLEN];
  size_t size;
  // I couldn't figure out why only the first conversation magically disappears somewhere.
  // So I enforce the silent first conversation.
  if (sendto(s, "", BUFLEN, 0, (struct sockaddr *)si_other, sizeof(struct sockaddr_in)) == -1)
      diep("sendto peer");
  while (1) {
    fgets(buffer, BUFLEN, stdin);
    if (sendto(s, buffer, BUFLEN, 0, (struct sockaddr *)si_other,
               sizeof(struct sockaddr_in)) == -1)
      diep("sendto peer");
  }
}

int main(int argc, char *argv[]) {
  struct sockaddr_in si_me, si_other;
  int i, f, j, k, slen = sizeof(si_other);
  struct client buf;
  struct client server;
  int n = 0;

  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    diep("socket");

  memset((char *)&si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  memset((char *)&si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  if (inet_aton(SRV_IP, &si_other.sin_addr) == 0)
    diep("aton");

  server.host = si_other.sin_addr.s_addr;
  server.port = si_other.sin_port;

  if (sendto(s, "i", 1, 0, (struct sockaddr *)(&si_other), slen) == -1)
    diep("sendto server");

  if (recvfrom(s, &buf, sizeof(buf), 0, (struct sockaddr *)(&si_other),
               &slen) == -1)
    diep("recvfrom server");
  printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr),
         ntohs(si_other.sin_port));
  if (server.host == si_other.sin_addr.s_addr &&
      server.port == (short)(si_other.sin_port)) {
    si_other.sin_addr.s_addr = buf.host;
    si_other.sin_port = buf.port;
    printf("Your peer %s:%d\n", inet_ntoa(si_other.sin_addr),
           ntohs(si_other.sin_port));
  } else {
    diep("recieve from unknown server");
  }

  pthread_t send_thread;
  if (pthread_create(&send_thread, NULL, send_message_to_peer, &si_other)) {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }

  while (1) {
    char bufc[BUFLEN];
    if (recvfrom(s, &bufc, sizeof(bufc), 0, (struct sockaddr *)(&si_other),
                 &slen) == -1)
      diep("recvfrom peer");

    printf("%s", bufc);
  }

  close(s);
  return 0;
}
