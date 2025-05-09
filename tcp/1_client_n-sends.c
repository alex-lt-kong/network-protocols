#include "common.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


int main(int argc, char const *argv[]) {
    struct timeval tp;
    int fd = 0;

    struct sockaddr_in serv_addr;
    char write_buffer[RW_BUF_SIZE];
    char read_buffer[RW_BUF_SIZE];
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("inet_pton()");
        goto err_inet_pton;
    }

    if (connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect()");
        goto err_connect;
    }
    for (int i = 0; i < 100; ++i) {
        gettimeofday(&tp, NULL);
        snprintf(write_buffer, sizeof(write_buffer) - 1, "msg from client at %d.%d", tp.tv_sec, tp.tv_usec);
        if (write(fd, write_buffer, strlen(write_buffer)) != -1) {
            memset(read_buffer, 0, RW_BUF_SIZE);
            // use send() instead of write() and use MSG_MORE to send messages in batches.
            // This, however, doesn't make much difference since we can accumulate the message and them
            // call send() once...
            const ssize_t bytes_read = read(fd, read_buffer, RW_BUF_SIZE);
            // sleep(1);
            printf("Message [%s] sent and reply [%s] received\n", write_buffer, read_buffer);
        } else {
            perror("send():");
        }
    }
err_connect:
err_inet_pton:
    close(fd);
    return 0;
}
