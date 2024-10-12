// Modeled after:
// https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6

#include "common.h"

volatile sig_atomic_t ev_flag;

int main() {
  uint64_t msg = 0;
  uint64_t prev_msg = 0;
  uint64_t first_msg;
  long long t0, t1;
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
    return 1;
  }
  struct sockaddr_in addr = prepare_receiver_addr();

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
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

  ev_flag = 0;
  (void)signal(SIGTERM, signal_handler);
  (void)signal(SIGINT, signal_handler);
  t0 = get_epoch_time_milliseconds();
  while (!ev_flag) {
    socklen_t addrlen = sizeof(addr);
    int nbytes =
        recvfrom(fd, &msg, sizeof(msg), 0, (struct sockaddr *)&addr, &addrlen);
    if (nbytes < 0) {
      perror("recvfrom()");
      break;
    }
    if (msg == 0) {
      first_msg = msg;
    } else {
      if (msg != prev_msg + 1) {
        fprintf(stderr, "Missed\n");
      }
    }
    prev_msg = msg;
    if (msg % (100 * 1000) == 0) {
      t1 = get_epoch_time_milliseconds();
      printf("%luK, %lld msg/s\n", msg / 1000,
             (msg - first_msg) * 1000 / (t1 - t0));
    }
  }
  printf("event loop exited gracefully\n");
  close(fd);
  return 0;
}