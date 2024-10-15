// Modeled after:
// https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6

#include "common.h"

#include <sys/time.h>
#include <threads.h>

volatile sig_atomic_t ev_flag;

int main(int argc, char **argv) {
  struct sockaddr_in in_addr;
  struct sockaddr_in sock_addr; /* Output structure from getsockname */
  int ret = 0;
  long long t0, t1;
  int fd;
  uint64_t msg_count = 0;
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Usage:\n  %s interface [--non-blocking, -n]\n", argv[0]);
    fprintf(stderr, "e.g., %s 192.168.0.100\n", argv[0]);
    return 1;
  }
  int non_blocking = (argc == 3 && (strcmp(argv[2], "--non-blocking") == 0 ||
                                    strcmp(argv[2], "-n") == 0));

  if (non_blocking) {
    printf("Sender in non-blocking mode\n");
    fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
  } else {
    printf("Sender in blocking mode\n");
    fd = socket(AF_INET, SOCK_DGRAM, 0);
  }
  if (fd < 0) {
    perror("socket()");
    return 1;
  }
  struct sockaddr_in addr = prepare_sender_addr();

  // Set up socket end-point info for binding
  memset(&in_addr, 0, sizeof(in_addr));
  in_addr.sin_family = AF_INET;                 /* Protocol domain */
  in_addr.sin_addr.s_addr = inet_addr(argv[1]); /* bind to interface */
  in_addr.sin_port = 0;                         /* Use any UDP port */
  if (bind(fd, (struct sockaddr *)&in_addr, sizeof(in_addr)) < 0) {
    perror("bind()");
    ret = 1;
    goto finalization;
  }
  socklen_t len = sizeof(sock_addr);
  getsockname(fd, (struct sockaddr *)&sock_addr, &len);
  printf("Socket fd is bound to:\n");
  printf("  addr = %s\n", inet_ntoa(sock_addr.sin_addr));
  printf("  port = %d\n", sock_addr.sin_port);

  ev_flag = 0;
  (void)signal(SIGTERM, signal_handler);
  (void)signal(SIGINT, signal_handler);
  t0 = get_epoch_time_milliseconds();
  while (!ev_flag) {
    ++msg_count;
    int nbytes = sendto(fd, &msg_count, sizeof(msg_count),
                        non_blocking ? MSG_DONTWAIT : 0,
                        (struct sockaddr *)&addr, sizeof(addr));
    if (nbytes < 0 && !non_blocking) {
      perror("sendto()");
      break;
    }
    if (msg_count % (10 * 1000) == 0) {
      t1 = get_epoch_time_milliseconds();
      printf("%" PRIu64 "K, %lld msg/s\n", msg_count / 1000,
             msg_count * 1000 / (t1 - t0));
    }
  }
  printf("event loop exited gracefully\n");
finalization:
  close(fd);
  return ret;
}