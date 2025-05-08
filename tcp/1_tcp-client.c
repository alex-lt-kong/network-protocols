// Client side C/C++ program to demonstrate Socket programming
#include "common.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  int fd = 0;
  struct sockaddr_in serv_addr;
  const char *default_payload = "Hello from client";
  char buffer[RW_BUF_SIZE] = {0};
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  memset(&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    perror("inet_pton():");
    goto err_inet_pton;
  }

  if (connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("connect():");
    goto err_connect;
  }
  if (argc == 2 && strlen(argv[1]) > 0)
    write(fd, argv[1], strlen(argv[1]));
  else
    write(fd, default_payload, strlen(default_payload));
  printf("Hello message sent\n");
  const ssize_t read_size = read(fd, buffer, RW_BUF_SIZE);
  if (read_size > 0)
    printf("%s\n", buffer);
err_connect:
err_inet_pton:
  close(fd);
  return 0;
}
