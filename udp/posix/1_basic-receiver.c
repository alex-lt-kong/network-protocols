// Modeled after:
// https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6

#include "common.h"

volatile sig_atomic_t ev_flag;

int main() {
  int ret = 0;
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
    ret = 1;
    goto finalization;
  }
  struct sockaddr_in addr = prepare_receiver_addr();

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    ret = 2;
    goto finalization;
  }

  // use setsockopt() to request the kernel to join a multicast mc_addr
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(mc_addr);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq,
                 sizeof(mreq)) < 0) {
    perror("setsockopt");
    ret = 3;
    goto finalization;
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
finalization:
  close(fd);
  return ret;
}