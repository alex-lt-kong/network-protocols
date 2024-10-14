// Modeled after:
// https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6

#include "common.h"

volatile sig_atomic_t ev_flag;

int main() {
  uint64_t msg = 0;
  uint64_t prev_msg = 0;
  uint64_t missed_msgs = 0;
  uint64_t t0_msg;
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

  // use setsockopt() to request the kernel to join a multicast mc_addr
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(mc_addr);
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
  t0 = -1;
  while (!ev_flag) {
    socklen_t addrlen = sizeof(addr);
    int nbytes =
        recvfrom(fd, &msg, sizeof(msg), 0, (struct sockaddr *)&addr, &addrlen);
    if (nbytes < 0) {
      perror("recvfrom()");
      continue;
    }

    if (msg != prev_msg + 1) {
      if (msg > prev_msg) {
        missed_msgs += (msg - prev_msg - 1);
        // printf("%lu - %lu = %lu\n", msg, prev_msg, msg - prev_msg);
      } else {
        fprintf(stderr,
                "msg < prev_msg (%" PRIu64 " vs %" PRIu64
                "), sender restarted?\n",
                msg, prev_msg);
        t0 = get_epoch_time_milliseconds();
        t0_msg = msg;
        missed_msgs = 0;
      }
    }

    prev_msg = msg;
    if (msg % (10 * 1000) == 0) {
      if (t0 == -1) {
        t0 = get_epoch_time_milliseconds();
        t0_msg = msg;
        continue;
      }
      t1 = get_epoch_time_milliseconds();
      // missed_msgs > msg - t0_msg means receiver starts later than sender
      if (missed_msgs < msg - t0_msg)
        printf("%" PRIu64
               "K, estimated sent: %lld msg/s, received: %lld msg/s, lost: "
               "%llu msg/s\n",
               msg / 1000, (msg - t0_msg) * 1000 / (t1 - t0),
               (msg - t0_msg - missed_msgs) * 1000 / (t1 - t0),
               missed_msgs * 1000 / (t1 - t0));
      else {
        t0 = get_epoch_time_milliseconds();
        t0_msg = msg;
        missed_msgs = 0;
      }
    }
  }
  printf("event loop exited gracefully\n");
  close(fd);
  return 0;
}