#include "common.h"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MAX_EVENTS 100

static volatile int ev_flag = 0;
int server_fd;
uint32_t connection_global_count = 0;

enum sock_type {
    LISTENING,
    CLIENT
};


struct sock_context {
    int fd;
    enum sock_type type;
    ssize_t conn_count;
};

// Signal handler for graceful shutdown
void handle_signal(int sig) {
    ev_flag = 1;
    // for a POSIX compliant system, shutdown() is guaranteed to be async-signal-safe
    // https://man7.org/linux/man-pages/man7/signal-safety.7.html
    shutdown(server_fd, SHUT_RDWR);
}

// Helper to set a file descriptor to nonblocking mode
int set_nonblocking(const int fd) {
    // man fcntl:
    // F_GETFL (void)
    // Return (as the function result) the file access mode and the file status flags; arg is ignored.
    const int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl()");
        return -1;
    }
    // man fcntl:
    // F_SETFL (int)
    // Set the file status flags to the value specified by arg.  File access mode (O_RDONLY, O_WRONLY, O_RDWR) and
    // file creation flags (i.e., O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC) in arg are ignored.  On Linux, this operation
    // can change only the O_APPEND, O_ASYNC, O_DIRECT, O_NOATIME, and O_NONBLOCK flags.  It is not possible to
    // change the O_DSYNC and O_SYNC flags; see BUGS, below.
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


void accept_new_connection(const int epoll_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    while (1) {
        const int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client_fd < 0) {
            // man accept:
            // EAGAIN or EWOULDBLOCK
            // The socket is marked nonblocking and no connections are present to be accepted.
            // POSIX.1-2001 and POSIX.1-2008 allow either error to be returned for this case,
            // and do not require these constants to have the same value, so  a  portable  application  should
            // check for both possibilities.
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            perror("accept()");
            break;
        }
        // Set the accepted socket to nonblocking mode
        if (set_nonblocking(client_fd) == -1) {
            perror("set_nonblocking()");
            close(client_fd);
            continue;
        }
        // Increase the global connection count and assign it to this connection
        connection_global_count++;
        struct sock_context *client_ctx = malloc(sizeof(struct sock_context));
        if (!client_ctx) {
            perror("malloc()");
            close(client_fd);
            continue;
        }
        client_ctx->fd = client_fd;
        client_ctx->type = CLIENT;
        client_ctx->conn_count = connection_global_count;

        printf("%u-th connection accepted from %s:%u, client_fd: %d\n",
               client_ctx->conn_count,
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               client_fd);

        // Register the new client socket with epoll for edge-triggered read events
        struct epoll_event client_event;
        client_event.events = EPOLLIN | EPOLLET;
        client_event.data.ptr = client_ctx;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1) {
            perror("epoll_ctl: client_fd");
            close(client_fd);
            free(client_ctx);
            continue;
        }
    }
}

void handle_existing_connection(struct sock_context *ctx) {
    const int client_fd = ctx->fd;
    char read_buffer[RW_BUF_SIZE];
    while (1) {
        ssize_t count = read(client_fd, read_buffer, sizeof(read_buffer) - 1);
        if (count == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break; // No more data to read
            perror("read()");
            close(client_fd);
            free(ctx);
            break;
        } else if (count == 0) {
            // Connection closed by client
            printf("%u-th client disconnected (fd %d).\n", ctx->conn_count, client_fd);
            close(client_fd);
            free(ctx);
            break;
        } else {
            read_buffer[count] = '\0'; // Null-terminate for printing
            printf("Received from %u-th connection (fd %d): %s\n",
                   ctx->conn_count, client_fd, read_buffer);

            // Compose a response that includes the connection number
            char write_buffer[RW_BUF_SIZE];
            int len = snprintf(write_buffer, sizeof(write_buffer),
                               "Message received from %u-th connection, you sent me: %s",
                               ctx->conn_count, read_buffer);
            int bytes_written = 0;
            while (bytes_written < len) {
                ssize_t written = write(client_fd, write_buffer + bytes_written, len - bytes_written);
                if (written == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        // For a robust server, you would monitor EPOLLOUT here.
                        break;
                    }
                    perror("write()");
                    close(client_fd);
                    free(ctx);
                    break;
                }
                bytes_written += written;
            }
        }
    }
}

int main() {
    int ret_val = EXIT_SUCCESS;
    struct sockaddr_in server_addr;

    if (signal(SIGINT, handle_signal) == SIG_ERR) {
        perror("signal()");
        ret_val = EXIT_FAILURE;
        goto err_signal;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket()");
        ret_val = EXIT_FAILURE;
        goto err_socket;
    }

    const int opt = 1;
    // https://www.ibm.com/docs/en/zos/3.1.0?topic=calls-setsockopt
    // SO_REUSEADDR
    // Toggles local address reuse. The default is disabled. This alters the normal algorithm used in the bind() call.

    // The normal bind() call algorithm allows each internet address and port combination to be bound only once. If the address and port have been bound already, a subsequent bind() will fail and return error EADDRINUSE.
    // After the 'SO_REUSEADDR' option is active, the following situations are supported:

    // - A server can bind() the same port multiple times as long as every invocation uses a different local IP address and the wildcard address INADDR_ANY is used only one time per port.
    // - A server with active client connections can be restarted and can bind to its port without having to close all of the client connections.
    // - For datagram sockets, multicasting is supported so multiple bind() calls can be made to the same class D address and port number.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt()");
        ret_val = EXIT_FAILURE;
        goto err_setsockopt;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind()");
        ret_val = EXIT_FAILURE;
        goto err_bind;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen()");
        ret_val = EXIT_FAILURE;
        goto err_listen;
    }

    printf("Server is listening on port %d...\n", PORT);

    if (set_nonblocking(server_fd) == -1) {
        perror("set_nonblocking()");
        ret_val = EXIT_FAILURE;
        goto err_set_nonblocking;
    }

    const int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1()");
        ret_val = EXIT_FAILURE;
        goto err_epoll_create1;
    }

    struct sock_context listen_ctx;
    listen_ctx.fd = server_fd;
    listen_ctx.type = LISTENING;
    listen_ctx.conn_count = -1;

    // Register the listening socket with epoll
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.ptr = &listen_ctx;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl(EPOLL_CTL_ADD)");
        ret_val = EXIT_FAILURE;
        goto err_epoll_ctl;
    }

    // Array to hold triggered events
    struct epoll_event events[MAX_EVENTS];

    // Main event loop
    while (!ev_flag) {
        const int fd_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
        if (fd_count < 0) {
            if (errno == EINTR)
                continue;
            perror("epoll_wait()");
            break;
        }
        for (int i = 0; i < fd_count; i++) {
            struct sock_context *ctx = (struct sock_context *) events[i].data.ptr;
            /*
            if (events[i].data.fd != ctx->fd)
                fprintf(stderr, "events[i].data.fd: %d, ctx->fd: %d\n", events[i].data.fd, ctx->fd);
                */
            // If this is the listening socket, accept new connections
            if (ctx->type == LISTENING) {
                accept_new_connection(epoll_fd);
            } else if (ctx->type == CLIENT) {
                handle_existing_connection(ctx);
            }
        }
    }

    printf("Server shutting down gracefully.\n");
err_epoll_ctl:
    close(epoll_fd);
err_epoll_create1:
err_set_nonblocking:
err_listen:
err_bind:
err_setsockopt:
    close(server_fd);
err_socket:
err_signal:
    return ret_val;
}
