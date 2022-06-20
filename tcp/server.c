#include <time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define READ_BUF_SIZE 2048

static volatile int keep_running = 1;

static size_t iso_dt_len = sizeof("1970-01-01T00:00:00Z");

char* get_iso_datetime(char* buf) {
    time_t now;
    time(&now);
    strftime(buf, iso_dt_len, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    return buf;
}

int main() {
    int server_fd; // _fd stands for file descriptor.
    char iso_dt[iso_dt_len];
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    /*
     * domain: AF_INET means the IP address family. AF means "Address Family". 
               Some other common AFs include AF_UNIX (unix socket)  and AF_BLUETOOTH (bluetooth)
     * type: SOCK_STREAM to open a stream socket (TCP), SOCK_DGRAM to open a datagram socket (UDP), and SOCK_RAW (IP).
     * protocol: For TCP/IP sockets, there’s only one form of virtual circuit service, so the last argument, protocol, is zero.
     */
    if (server_fd < 0) {
        perror("cannot create socket");
        return 1; 
    }

    /* Bing a socket */
    struct sockaddr_in address; // _in stands for INternet
    /* Because sockets were designed to work with various different types of communication interfaces, the interface
     * is very general. Instead of accepting, say, a port number as a parameter, it takes a sockaddr structure whose
     * actual format is determined on the address family (type of network) you're using.
     */
    int addrlen = sizeof(address);
    char *resp = "Hello from server";
    const int PORT = 8080; //Where the clients can reach at

    /* htonl converts a long integer (e.g. address) to a network representation */
    memset((char*)&address, 0, sizeof(address)); 
    address.sin_family = AF_INET;
    // sin_family is always set to AF_INET.  This is required. sin stands for sockaddr_in.
    address.sin_addr.s_addr = htonl(INADDR_ANY); 
    // It does NOT "generate a random IP". It binds the socket to all available interfaces, i.e., binds to 0.0.0.0.
    address.sin_port = htons(PORT);
    // port in network byte order. htons() translates a short integer from host byte order to network byte order

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { 
        /* To bind/name a socket, means assigning a transport address to the socket. In the case of IP networking, it
         * is a port number. In sockets, this operation is called binding an address.
         */
        perror("In bind()"); 
        return 0; 
    }

    if (listen(server_fd, 10) < 0) {
        /* listen() marks the socket referred to by sockfd as a passive socket, that is, as a socket that will be
         * used to accept incoming connection requests using accept().
         * sockfd: a file descriptor that refers to a socket of type SOCK_STREAM or SOCK_SEQPACKET.
         * backlog, defines the maximum number of pending connections that can be queued up before connections are refused.
         *
         * When listen(2) is called on an unbound socket, the socket is automatically bound to a random free port with the
         * local address set to INADDR_ANY.
        */
        perror("In listen()");
        exit(EXIT_FAILURE);
    }
    printf("[%s] Listening on 0.0.0.0:%d\n", get_iso_datetime(iso_dt), PORT);
    struct sockaddr_in from;
    socklen_t fromlen = sizeof (from);
    int new_socket;
    while(1) {
        new_socket = accept(server_fd, (struct sockaddr*)&from, (socklen_t*)&fromlen);
        /* The accept system call grabs the first connection request on the queue of pending connections
         *  (set up in listen) and creates a new socket for that connection.
         * from and fromlen doesnt appear to be a must.
         * The original socket that was set up for listening is used only for accepting connections,
         * not for exchanging data. By default, socket operations are synchronous, and accept will block until
         * a connection is present on the queue.
         */
        if (new_socket < 0) {
            perror("In accept()");
            exit(EXIT_FAILURE);
        }
        /* write()/send() and read()/recv() are almost idenitical except that send() and recv() receive an extra
         * paramter flag, giving us more flexibility
         */
        char buffer[READ_BUF_SIZE] = {0};
        if (recv(new_socket, buffer, READ_BUF_SIZE, 0) != -1) {
            printf("[%s] Received: %s\n", get_iso_datetime(iso_dt), buffer);            
        } else {
            perror("In read()");
        }

        if (write(new_socket, resp ,strlen(resp)) != -1) {
            printf("[%s] Sent: %s\n", get_iso_datetime(iso_dt), resp);
        } else {
            perror("In write()");
        }
        
        close(new_socket);
    }
    close(server_fd);
    return 0;
}