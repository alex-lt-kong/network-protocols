#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 512

const char group[] = "239.0.0.1";
const int port = 5555;

extern volatile sig_atomic_t ev_flag;

void signal_handler(int signum) {
  (void)(signum);
  ev_flag = 1;
}
#endif /* COMMON_H */