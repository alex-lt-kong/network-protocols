#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 512

const char group[] = "239.0.0.1";
// const char group[] = "127.0.0.1";
const int port = 5555;

extern volatile sig_atomic_t ev_flag;

static inline void signal_handler(int signum) {
  signum %= 100;
  char msg[] = "Signal [  ] caught\n";
  msg[8] = '0' + (char)(signum / 10);
  msg[9] = '0' + (char)(signum % 10);
  write(STDIN_FILENO, msg, strlen(msg));
  ev_flag = 1;
}

static inline struct sockaddr_in prepare_sender_addr() {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(group);
  addr.sin_port = htons(port);
  return addr;
}

static inline struct sockaddr_in prepare_receiver_addr() {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  // differs from sender, but group is still specified below
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  return addr;
}

static inline long long get_epoch_time_milliseconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)(tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

#endif /* COMMON_H */