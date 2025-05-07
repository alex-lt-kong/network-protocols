#include "common.h"

#include <signal.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

static volatile uint32_t connection_count;
static volatile int ev_flag = 0;
int server_fd;

void handle_signal(int sig) {
    ev_flag = 1;
    shutdown(server_fd, SHUT_RDWR);
}

void *handle_client(void *arg) {
    const int client_fd = *(int *) arg;
    char buffer[RW_BUF_SIZE];
    char write_buffer[RW_BUF_SIZE];
    free(arg);
    uint32_t conn_count = ++connection_count;

    struct sockaddr_in server_addr;

    socklen_t addr_len = sizeof(server_addr);
    if (getsockname(client_fd, (struct sockaddr *) &server_addr, &addr_len) == -1) {
        perror("getsockname()");
        return NULL;
    }
    struct sockaddr_in client_addr;
    addr_len = sizeof(client_addr);
    if (getpeername(client_fd, (struct sockaddr *) &client_addr,
                    &addr_len) == -1) {
        perror("getpeername()");
        return NULL;
    }

    printf("Connection accept()'ed. server: %s:%d <-> client %s:%d, fd: %d\n",
           inet_ntoa(server_addr.sin_addr),
           ntohs(server_addr.sin_port), inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port), client_fd);
    while (1) {
        memset(buffer, 0, RW_BUF_SIZE);
        const ssize_t bytes = read(client_fd, buffer, RW_BUF_SIZE);
        if (bytes > 0) {
            printf("Received from %d-th connection: %s\n", conn_count, buffer);
            snprintf(write_buffer, RW_BUF_SIZE, "Message received, you sent me: [%s] and you are %d-th connection",
                     buffer, conn_count);
            write(client_fd, write_buffer, strnlen(write_buffer, RW_BUF_SIZE));
        } else if (bytes == 0) {
            printf("%d-th client disconnected.\n", conn_count);
            break;
        } else {
            perror("read()");
            break;
        }
    }

    close(client_fd);
    return NULL;
}

int main() {
    signal(SIGINT, handle_signal);
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket()");
        return 1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        /* To bind/name a socket, means assigning a transport address to the socket.
         * In the case of IP networking, it is a port number. In sockets, this
         * operation is called binding an address.
         */
        perror("bind()");
        goto err_bind;
    }
    if (listen(server_fd, 10) < 0) {
        perror("listen()");
        goto err_listen;
    }

    printf("Server is listening on port %d...\n", PORT);

    while (!ev_flag) {
        const int new_socket_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_len);
        if (new_socket_fd < 0) {
            if (!ev_flag) {
                perror("accept() failed");
                continue;
            }
            printf("accept() returned\n");
        }

        pthread_t tid;
        int *client_sock = malloc(sizeof(int));
        *client_sock = new_socket_fd;

        pthread_create(&tid, NULL, handle_client, client_sock);
        pthread_detach(tid);
    }
    printf("event loop exited gracefully\n");

err_bind:
err_listen:
    close(server_fd);
    return 0;
}
