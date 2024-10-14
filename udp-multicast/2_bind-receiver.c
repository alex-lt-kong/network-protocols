// Modeled after:
// https://www.cs.emory.edu/~cheung/Courses/558/Syllabus/14-Multicast/Progs/mcast-send.c
#include "common.h"

volatile sig_atomic_t ev_flag;

int main() {
  struct sockaddr_in in_addr;		
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    perror("socket");
    return 1;
  }

  int yes = 1;
  // allow multiple applications to receive datagrams that are destined to the
  // same local port number.
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0) {
    perror("Reusing ADDR failed");
    close(fd);
    return 1;
  }
  struct sockaddr_in addr = prepare_receiver_addr();

  // Set up socket end-point info for binding
  in_addr.sin_family = AF_INET;                           /* Protocol domain */
  in_addr.sin_addr.s_addr = 0;   /* Use wildcard IP address */
  in_addr.sin_port = htons(port);           	                      /* Use any UDP port */
  if (bind(fd, (struct sockaddr *)&in_addr, sizeof(in_addr)) < 0) {
    perror("bind()");
    close(fd);
    return 1;
  }

  // use setsockopt() to request the kernel to join a multicast group
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(group);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq,
                 sizeof(mreq)) < 0) {
    perror("setsockopt");
    return 1;
  }
  struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
  if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    perror("setsockopt");
  }

  ev_flag = 0;
  (void)signal(SIGTERM, signal_handler);
  (void)signal(SIGINT, signal_handler);
  while (!ev_flag) {
    char msgbuf[BUFSIZE];
    socklen_t addrlen = sizeof(addr);
    int nbytes =
        recvfrom(fd, msgbuf, BUFSIZE, 0, (struct sockaddr *)&addr, &addrlen);
    if (nbytes < 0) {
      perror("recvfrom()");
      continue;
    }
    msgbuf[nbytes] = '\0';
    puts(msgbuf);
  }
  printf("event loop exited gracefully\n");
  close(fd);
  return 0;
}