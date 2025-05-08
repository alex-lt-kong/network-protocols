#include "common.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_EVENTS 64

static volatile int ev_flag = 0;
int server_fd;
int conn_count = 0; // Total connections counter

// Structure to hold connection context
typedef struct {
    int fd; // Client file descriptor
    int snapshot_id; // Snapshot of conn_count when connected
} connection_context_t;

// Simple associative storage for connection contexts
#define MAX_CONNECTIONS 1024
connection_context_t connections[MAX_CONNECTIONS];

// Signal handler to gracefully shut down the server
void handle_signal(int sig) {
    ev_flag = 1;
    shutdown(server_fd, SHUT_RDWR);
}

// Function to set a socket to non-blocking mode
void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// Function to store the connection snapshot
void store_connection_context(int fd) {
    conn_count++; // Increment for each new connection
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].fd == 0) {
            // Find empty slot
            connections[i].fd = fd;
            connections[i].snapshot_id = conn_count;
            return;
        }
    }
}

// Function to get a connection's snapshot
int get_connection_snapshot(int fd) {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].fd == fd) {
            return connections[i].snapshot_id;
        }
    }
    return -1; // Should never happen if connections are tracked correctly
}

// Function to remove context when connection closes
void remove_connection_context(int fd) {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].fd == fd) {
            connections[i].fd = 0;
            connections[i].snapshot_id = 0;
            return;
        }
    }
}

int main() {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    struct epoll_event ev, events[MAX_EVENTS];

    if (signal(SIGINT, handle_signal) == SIG_ERR) {
        perror("signal()");
        goto cleanup;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket()");
        goto cleanup;
    }

    set_nonblocking(server_fd);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &addr, addr_len) < 0) {
        perror("bind()");
        goto cleanup;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen()");
        goto cleanup;
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1()");
        goto cleanup;
    }

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) < 0) {
        perror("epoll_ctl()");
        goto cleanup;
    }

    printf("Listening on 0.0.0.0:%d\n", PORT);

    while (!ev_flag) {
        const int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait()");
            break;
        }

        for (int i = 0; i < num_events; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                while (1) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_len);
                    if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        perror("accept()");
                        break;
                    }

                    set_nonblocking(client_fd);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = client_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

                    store_connection_context(client_fd); // Store client's snapshot
                    printf("New connection: %s:%d (fd: %d) | Connection snapshot: %d\n",
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd, conn_count);
                }
            } else {
                char buffer[RW_BUF_SIZE] = {0};
                ssize_t bytes_read = read(fd, buffer, RW_BUF_SIZE);
                if (bytes_read > 0) {
                    int snapshot = get_connection_snapshot(fd);
                    char response[RW_BUF_SIZE];
                    snprintf(response, RW_BUF_SIZE, "Your connection snapshot: %d\n", snapshot);
                    write(fd, response, strlen(response));
                } else if (bytes_read == 0) {
                    close(fd);
                    remove_connection_context(fd);
                    printf("Client (fd: %d) disconnected\n", fd);
                } else {
                    perror("read()");
                }
            }
        }
    }

cleanup:
    close(server_fd);
    close(epoll_fd);
    printf("Server shutting down...\n");
    return 0;
}
