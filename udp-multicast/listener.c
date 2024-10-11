// Modeled after:
// https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6

#include "common.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 512

int main() {
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    perror("socket");
    return 1;
  }

  // allow multiple sockets to use the same PORT number
  //
  u_int yes = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0) {
    perror("Reusing ADDR failed");
    return 1;
  }

  // set up destination address
  //
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY); // differs from sender
  addr.sin_port = htons(port);

  // bind to receive address
  //
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    return 1;
  }

  // use setsockopt() to request that the kernel join a multicast group
  //
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(group);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq,
                 sizeof(mreq)) < 0) {
    perror("setsockopt");
    return 1;
  }

  // now just enter a read-print loop
  //
  while (1) {
    char msgbuf[BUFSIZE];
    int addrlen = sizeof(addr);
    int nbytes =
        recvfrom(fd, msgbuf, BUFSIZE, 0, (struct sockaddr *)&addr, &addrlen);
    if (nbytes < 0) {
      perror("recvfrom");
      return 1;
    }
    msgbuf[nbytes] = '\0';
    puts(msgbuf);
  }

#ifdef _WIN32
  //
  // Program never actually gets here due to infinite loop that has to be
  // canceled, but since people on the internet wind up using examples
  // they find at random in their own code it's good to show what shutting
  // down cleanly would look like.
  //
  WSACleanup();
#endif

  return 0;
}