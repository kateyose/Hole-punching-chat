#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFLEN 512
#define PORT 6868

struct client {
  int host;
  short port;
};

void diep(char *s) {
  perror(s);
  exit(1);
}

int main(void) {
  struct sockaddr_in si_me, si_other;
  int s, i, j, slen = sizeof(si_other);
  char buf[BUFLEN];
  struct client clients[2];
  int n = 0;

  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    diep("socket");

  memset((char *)&si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(s, (struct sockaddr *)(&si_me), sizeof(si_me)) == -1)
    diep("bind");

  while (1) {
    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)(&si_other), &slen) ==
        -1)
      diep("recvfrom");

    printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr),
           ntohs(si_other.sin_port));
    clients[n].host = si_other.sin_addr.s_addr;
    clients[n].port = si_other.sin_port;
    n++;
    if (n == 2) {
      printf("introduce themselves each other");
      for (i = 0; i < n; i++) {
        si_other.sin_addr.s_addr = clients[i].host;
        si_other.sin_port = clients[i].port;
        printf("Sending to %s:%d\n", inet_ntoa(si_other.sin_addr),
               ntohs(si_other.sin_port));
        if (sendto(s, &clients[i ? 0 : 1], 6, 0, (struct sockaddr *)(&si_other),
                   slen) == -1)
          diep("sendto");
      }
      n = 0;
    }
  }

  close(s);
  return 0;
}
