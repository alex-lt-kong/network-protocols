// Modeled after:
// https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6

#include "common.h"

#include <sys/time.h>
#include <threads.h>

volatile sig_atomic_t ev_flag;

int main() {
  uint64_t msg_count = 0;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    perror("socket()");
    return 1;
  }
  struct sockaddr_in addr = prepare_sender_addr();

  ev_flag = 0;
  (void)signal(SIGTERM, signal_handler);
  (void)signal(SIGINT, signal_handler);
  while (!ev_flag) {
    ++msg_count;
    int nbytes = sendto(fd, &msg_count, sizeof(msg_count), 0,
                        (struct sockaddr *)&addr, sizeof(addr));
    if (nbytes < 0) {
      perror("sendto()");
      break;
    }
  }
  printf("event loop exited gracefully\n");
  close(fd);
  return 0;
}