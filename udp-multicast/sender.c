// Modeled after:
// https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6

#include "common.h"

#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long long get_epoch_time_milliseconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)(tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

int main() {
  char message[BUFSIZE];
  size_t msg_count = 0;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    perror("socket");
    return 1;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(group);
  addr.sin_port = htons(port);

  while (1) {
    sprintf(
        message,
        "[%zu] Hello, World  from 0xDEADBEEF at %lld! This is a relative long "
        "payload stamped by current time",
        ++msg_count, get_epoch_time_milliseconds());
    int nbytes = sendto(fd, message, strlen(message), 0,
                        (struct sockaddr *)&addr, sizeof(addr));
    if (nbytes < 0) {
      perror("sendto");
      return 1;
    }
  }
  return 0;
}