// Modeled after:
// https://www.cs.emory.edu/~cheung/Courses/558/Syllabus/14-Multicast/Progs/mcast-send.c

#include "common.h"

volatile sig_atomic_t ev_flag;

int main() {
  struct sockaddr_in in_addr;		
  struct sockaddr_in sock_addr;	/* Output structure from getsockname */
  char message[BUFSIZE];
  size_t msg_count = 0;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    perror("socket()");
    return 1;
  }
  struct sockaddr_in addr = prepare_sender_addr();

  // Set up socket end-point info for binding
  in_addr.sin_family = AF_INET;                           /* Protocol domain */
  in_addr.sin_addr.s_addr = inet_addr("192.168.199.2");   /* Use wildcard IP address */
  in_addr.sin_port = 0;	           	                      /* Use any UDP port */
  if (bind(fd, (struct sockaddr *)&in_addr, sizeof(in_addr)) < 0) {
    perror("bind()");
    close(fd);
    return 1;
  }
  socklen_t len = sizeof(sock_addr);
  getsockname(fd, (struct sockaddr *) &sock_addr, &len);
  printf("Socket s is bind to:\n");
  printf("  addr = %u\n", sock_addr.sin_addr.s_addr);
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
  }
  printf("event loop exited gracefully\n");
  close(fd);
  return 0;
}