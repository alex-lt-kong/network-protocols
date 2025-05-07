// Modeled after:
// https://www.cs.emory.edu/~cheung/Courses/558/Syllabus/14-Multicast/Progs/mcast-send.c

#include "common.h"

volatile sig_atomic_t ev_flag;

int main(int argc, char **argv) {
  int ret = 0;
  struct sockaddr_in in_addr;
  struct sockaddr_in sock_addr; /* Output structure from getsockname */
  int fd;
  if (argc != 2) {
    fprintf(stderr, "Usage:\n  %s interface\n", argv[0]);
    fprintf(stderr, "e.g., %s 192.168.0.100\n", argv[0]);
    return 1;
  }
  char message[BUFSIZE];
  size_t msg_count = 0;
  fd = socket(AF_INET, SOCK_DGRAM, 0);
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
  while (!ev_flag) {
    sprintf(
        message,
        "[%zu] Hello, World  from 0xDEADBEEF at %lld! This is a relative long "
        "payload stamped by current time",
        ++msg_count, get_epoch_time_milliseconds());
    int nbytes = sendto(fd, message, strlen(message), 0,
                        (struct sockaddr *)&addr, sizeof(addr));
    if (nbytes < 0) {
      perror("sendto()");
      break;
    }
    printf("msg sent: %s\n", message);
    sleep(1);
  }
  printf("event loop exited gracefully\n");
finalization:
  close(fd);
  return ret;
}