/* This server can handle one connection at a time, if a second client tries to connect, it will  have to wait until
 * the first client disconnects */
#include   "common.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static volatile int ev_flag = 0;
int server_fd;

void handle_signal(int sig) {
    ev_flag = 1;
    shutdown(server_fd, SHUT_RDWR);
}

int main() {
    // _fd stands for file descriptor.
    char iso_dt[iso_dt_len];

    /*
     * domain: AF_INET means the IP address family. AF means "Address Family".
               Some other common AFs include AF_UNIX (unix socket)  and
               AF_BLUETOOTH (bluetooth)
     * type: SOCK_STREAM to open a stream socket (TCP), SOCK_DGRAM to open a
             datagram socket (UDP), and SOCK_RAW (IP).
     * protocol: For TCP/IP sockets, there’s only one form of virtual circuit
                 service, so the last argument, protocol, is zero.
     */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("socket()");
        return 1;
    }

    /* Bing a socket */
    struct sockaddr_in addr; // _in stands for INternet
    socklen_t addr_len = sizeof(addr);
    /* Because sockets were designed to work with various different types of
     * communication interfaces, the interface is very general. Instead of
     * accepting, say, a port number as a parameter, it takes a sockaddr structure
     * whose actual format is determined on the address family (type of network)
     * you're using.
     */
    // int addrlen = sizeof(address);

    memset((char *) &addr, 0, sizeof(addr));
    // sin_family is always set to AF_INET.  This is required. sin stands for
    // sockaddr_in.
    addr.sin_family = AF_INET;
    /* htonl converts a long integer (e.g. address) to a network representation */
    // It does NOT "generate a random IP". It binds the socket to all available
    // interfaces, i.e., binds to 0.0.0.0.
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // port in network byte order. htons() translates a short integer from host
    // byte order to network byte order
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &addr, addr_len) < 0) {
        /* To bind/name a socket, means assigning a transport address to the socket.
         * In the case of IP networking, it is a port number. In sockets, this
         * operation is called binding an address.
         */
        perror("bind()");
        return 0;
    }

    if (listen(server_fd, 10) < 0) {
        /* listen() marks the socket referred to by sockfd as a passive socket, that
         * is, as a socket that will be used to accept incoming connection requests
         * using accept(). sockfd: a file descriptor that refers to a socket of type
         * SOCK_STREAM or SOCK_SEQPACKET. backlog, defines the maximum number of
         * pending connections that can be queued up before connections are refused.
         *
         * When listen(2) is called on an unbound socket, the socket is
         * automatically bound to a random free port with the local address set to
         * INADDR_ANY.
         */
        perror("In listen()");
        exit(EXIT_FAILURE);
    }
    printf("[%s] Listening on 0.0.0.0:%d\n", get_iso_datetime(iso_dt), PORT);
    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);
    const char resp[128] = "Acknowledged";
    char buffer[RW_BUF_SIZE];
    ssize_t bytes = -1;
    while (!ev_flag) {
        memset(buffer, 0, RW_BUF_SIZE);

        /* The accept system call grabs the first connection request on the queue of
         * pending connections (known as an "accept queue" in Linux) and creates a
         * new socket for that connection. from and fromlen doesnt appear to be a
         * must. The original socket that was set up for listening is used only for
         * accepting connections, not for exchanging data. By default, socket
         * operations are synchronous, and accept will block until a connection is
         * present on the queue.
         */
        const int new_socket_fd =
                accept(server_fd, (struct sockaddr *) &from, &from_len);
        if (new_socket_fd < 0) {
            if (!ev_flag) {
                perror("accept() failed");
                continue;
            }
            printf("accept() returned\n");
        }

        if (getsockname(new_socket_fd, (struct sockaddr *) &addr, &addr_len) == -1) {
            perror("In getsockname()");
            exit(EXIT_FAILURE);
        }
        struct sockaddr_in client_addr;
        addr_len = sizeof(client_addr);
        if (getpeername(new_socket_fd, (struct sockaddr *) &client_addr,
                        &addr_len) == -1) {
            perror("In getpeername()");
            exit(EXIT_FAILURE);
        }

        printf("[%s] Connection accept()'ed. server: %s:%d <-> client %s:%d, fd: %d\n",
               get_iso_datetime(iso_dt), inet_ntoa(addr.sin_addr),
               ntohs(addr.sin_port), inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), new_socket_fd);

        /* write()/send() and read()/recv() are almost identical except that send()
         * and recv() receive an extra parameter flag, giving us more flexibility
         */
        do {
            // recv() blocks the loop until any data is readable.
            bytes = read(new_socket_fd, buffer, RW_BUF_SIZE);
            if (bytes > 0) {
                printf("[%s] Received: %s\n", get_iso_datetime(iso_dt), buffer);
                write(new_socket_fd, resp, strlen(resp));
            } else if (bytes == 0) {
                break;
                // If no messages are available to be received and the peer has
                // performed an orderly shutdown, recv() shall return 0
            } else {
                perror("In recv()");
            }
        } while (1);

        printf("[%s] Connection close()'ed\n", get_iso_datetime(iso_dt));

        close(new_socket_fd);
    }
    close(server_fd);
    return 0;
}
