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

// For global/static mutex, we use PTHREAD_MUTEX_INITIALIZER to replace pthread_mutex_init()
pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint32_t connection_count;
static volatile int ev_flag = 0;
int server_fd;

void handle_signal(int sig) {
    ev_flag = 1;
    // for a POSIX compliant system, shutdown() is guaranteed to be async-signal-safe
    // https://man7.org/linux/man-pages/man7/signal-safety.7.html
    shutdown(server_fd, SHUT_RDWR);
}

void *handle_client(void *arg) {
    const int client_fd = *(int *) arg;
    char read_buffer[RW_BUF_SIZE];
    char write_buffer[RW_BUF_SIZE];
    free(arg);
    int res;
    if ((res = pthread_mutex_lock(&connection_mutex)) != 0) {
        fprintf(stderr, "pthread_mutex_lock() failed: %s\n", strerror(res));
        return NULL;
    }
    const uint32_t conn_count = ++connection_count;
    if ((res = pthread_mutex_unlock(&connection_mutex)) != 0) {
        fprintf(stderr, "pthread_mutex_unlock() failed: %s\n", strerror(res));
        ev_flag = 1;
        return NULL;
    }

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

    printf("%d-th connection accept()'ed. server: %s:%d <-> client %s:%d, fd: %d\n", conn_count,
           inet_ntoa(server_addr.sin_addr),
           ntohs(server_addr.sin_port), inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port), client_fd);
    while (1) {
        memset(read_buffer, 0, RW_BUF_SIZE);
        const ssize_t read_size = read(client_fd, read_buffer, RW_BUF_SIZE);

        /* Think about it: what would be the expected behavior if we dont discard the remaining bytes?
        while (read_size == RW_BUF_SIZE) {
            char discard_buffer[RW_BUF_SIZE];
            const ssize_t discard_size = read(client_fd, discard_buffer, RW_BUF_SIZE);
            if (discard_size <= 0) {
                break;
            }
        }
        */

        if (read_size == RW_BUF_SIZE)
            printf("Warning: more bytes could have been discarded in read()\n");
        if (read_size > 0) {
            printf("Received from %d-th connection: %s\n", conn_count, read_buffer);
            memset(write_buffer, 0, RW_BUF_SIZE);
            snprintf(write_buffer, RW_BUF_SIZE, "Message received from %d-th connection, you sent me: ", conn_count);
            ssize_t expected_write_size = strlen(write_buffer);
            memcpy(write_buffer + expected_write_size, read_buffer, RW_BUF_SIZE - expected_write_size - 1);
            expected_write_size += RW_BUF_SIZE - expected_write_size - 1;
            write_buffer[expected_write_size] = '\0'; // Not strictly needed, but can make life easier for client
            if (write(client_fd, write_buffer, expected_write_size) != expected_write_size) {
                fprintf(stderr, "write() error");
            }
        } else if (read_size == 0) {
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
    if (signal(SIGINT, handle_signal) == SIG_ERR) {
        perror("signal()");
        goto err_signal;
    }
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

        const int res = pthread_create(&tid, NULL, handle_client, client_sock);
        if (res == 0)
            pthread_detach(tid);
        else {
            // Easy-to-miss manual release--spotted by Bing Copilot
            fprintf(stderr, "pthread_create() failed: %s\n", strerror(res));
            free(client_sock);
        }
    }
    printf("event loop exited gracefully\n");

err_bind:
err_listen:
    close(server_fd);
    // Current flow is problematic--if we interrupt the event loop before all connections are closed, some existing
    // connections could try to lock the mutex that has been destroyed
    pthread_mutex_destroy(&connection_mutex);
err_signal:
    return 0;
}
