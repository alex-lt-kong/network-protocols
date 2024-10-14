#include "common.h"

#include <sys/time.h>
// #include <threads.h>

volatile sig_atomic_t ev_flag;

int main() {
  long long t0, t1;
  uint64_t msg_count = 0;
  int fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
  if (fd < 0) {
    perror("socket()");
    return 1;
  }
  struct sockaddr_in addr = prepare_sender_addr();

  ev_flag = 0;
  (void)signal(SIGTERM, signal_handler);
  (void)signal(SIGINT, signal_handler);
  t0 = get_epoch_time_milliseconds();
  while (!ev_flag) {
    ++msg_count;
    (void)sendto(fd, &msg_count, sizeof(msg_count), MSG_DONTWAIT,
                 (struct sockaddr *)&addr, sizeof(addr));
    if (msg_count % (1000 * 1000) == 0) {
      t1 = get_epoch_time_milliseconds();
      printf("%" PRIu64 " mil, %lld msg/s\n", msg_count / 1000 / 1000,
             msg_count * 1000 / (t1 - t0));
    }
  }
  printf("event loop exited gracefully\n");
  close(fd);
  return 0;
}