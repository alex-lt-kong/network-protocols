// Modeled after:
// https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6 Note
// that what this program does should be equivalent to NETCAT:
//
//     echo "Hello World" | nc -u 239.255.255.250 1900

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h> // for sleep()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 512

long long get_epoch_time_milliseconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)(tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("%s ip port\n", argv[0]);
    printf("(e.g. for SSDP, `%s 239.255.255.250 1900`)\n", argv[0]);
    return 1;
  }

  const char *group = argv[1]; // e.g. 239.255.255.250 for SSDP
  int port = atoi(argv[2]);    // 0 if error, which is an invalid port
  const int delay_secs = 1;
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