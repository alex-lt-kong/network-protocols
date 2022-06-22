#include <time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define HEADER_BUF_SIZE 4096
#define READ_BUF_SIZE 2048
#define HTTP_PORT 8081

static volatile int keep_running = 1;

static size_t iso_dt_len = sizeof("1970-01-01T00:00:00Z");

char* get_iso_datetime(char* buf) {
    time_t now;
    time(&now);
    strftime(buf, iso_dt_len, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    return buf;
}

int main() {
    int server_fd;
    char iso_dt[iso_dt_len];
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("cannot create socket");
        return 1; 
    }
    struct sockaddr_in address;    
    int addrlen = sizeof(address);
    char header[HEADER_BUF_SIZE] = {0};
    char* resp = NULL;

    FILE *file;
    char *file_buffer;
    size_t file_len = 0, resp_len = 0;

    //Open file
    file = fopen("server-file.mp4", "rb");
    if (!file) {
        perror("In fopen()");
        return EXIT_FAILURE;
    }

    //Get file length
    fseek(file, 0, SEEK_END);
    file_len = ftell(file);
    fseek(file, 0, SEEK_SET);

    
    file_buffer=(unsigned char*)malloc(file_len+1);
    if (!file_buffer) {
        perror("In malloc()");        
        fclose(file);
        return EXIT_FAILURE;
    }
    
    snprintf(header, HEADER_BUF_SIZE,
        "HTTP/1.1 200 OK\n"     
        "Server: Snakeoil/2.2.3\n"
        "Content-Type: video/mp4\n"
        "Content-Length: %d\n"
        "Accept-Ranges: bytes\n"
        "Connection: close\n"
        "\n",
    file_len);
     //Read file contents into buffer
    fread(file_buffer, file_len, 1, file);
    fclose(file);

    
    resp = (unsigned char*)malloc(strlen(header) + file_len);
    strcpy(resp, header);
    memcpy(resp + strlen(header), file_buffer, file_len);
    resp_len = strlen(header) + file_len;
    free(file_buffer);
    
    
    /* htonl converts a long integer (e.g. address) to a network representation */
    memset((char*)&address, 0, sizeof(address)); 
    address.sin_family = AF_INET;
    // sin_family is always set to AF_INET.  This is required. sin stands for sockaddr_in.
    address.sin_addr.s_addr = htonl(INADDR_ANY); 
    // It does NOT "generate a random IP". It binds the socket to all available interfaces, i.e., binds to 0.0.0.0.
    address.sin_port = htons(HTTP_PORT);
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
    printf("[%s] Listening on 0.0.0.0:%d\n", get_iso_datetime(iso_dt), HTTP_PORT);
    int new_socket;
    while(1) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        printf("New connection accept()'ed");
        if (new_socket < 0) {            
            perror("In accept()");
            free(resp);
            exit(EXIT_FAILURE);
        }
        
        char buffer[READ_BUF_SIZE] = {0};
        if (read(new_socket, buffer, READ_BUF_SIZE) != -1) {
            printf("[%s] Received: [\n%s]\n", get_iso_datetime(iso_dt), buffer);
            if (write(new_socket, resp ,resp_len) != -1) {
                printf("Response sent");
            } else {
                perror("In write()");
            }
        } else {
            perror("In read()");
        }
        
        close(new_socket);
    }
    close(server_fd);
    return 0;
}