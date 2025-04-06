// Modeled after:
// https://www.cs.emory.edu/~cheung/Courses/558/Syllabus/14-Multicast/Progs/mcast-recv.c
// Also refer to:
// https://stackoverflow.com/questions/12681097/c-choose-interface-for-udp-multicast-socket
#include "common.h"

volatile sig_atomic_t ev_flag;

int main(int argc, char **argv) {
  int ret = 0;
  struct sockaddr_in src_addr;
  if (argc != 2) {
    fprintf(stderr, "Usage:\n  %s interface\n", argv[0]);
    fprintf(stderr, "e.g., %s 192.168.0.100\n", argv[0]);
    return 1;
  }
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    perror("socket()");
    return 1;
  }

  int yes = 1;
  // allow multiple applications to receive datagrams that are destined to the
  // same local port number.
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0) {
    perror("setsockopt(SO_REUSEADDR)");
    ret = 1;
    goto finalization;
  }
  src_addr = prepare_receiver_addr();
  if (bind(fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
    perror("bind()");
    ret = 2;
    goto finalization;
  }

  // use setsockopt() to request the kernel to join a multicast group
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(mc_addr);
  mreq.imr_interface.s_addr = inet_addr(argv[1]);
  // Add multicast membership...
  if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq,
                 sizeof(mreq)) < 0) {
    perror("setsockopt(IP_ADD_MEMBERSHIP)");
    ret = 3;
    goto finalization;
  }

  struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
  if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    perror("setsockopt(SO_RCVTIMEO)");
    ret = 4;
    goto finalization;
  }

  ev_flag = 0;
  (void)signal(SIGTERM, signal_handler);
  (void)signal(SIGINT, signal_handler);
  while (!ev_flag) {
    char msgbuf[BUFSIZE];
    socklen_t addrlen = sizeof(src_addr);
    int nbytes = recvfrom(fd, msgbuf, BUFSIZE, 0, (struct sockaddr *)&src_addr,
                          &addrlen);
    if (nbytes < 0) {
      perror("recvfrom()");
      continue;
    }
    msgbuf[nbytes] = '\0';
    puts(msgbuf);
  }
  printf("event loop exited gracefully\n");
finalization:
  close(fd);
  return ret;
}