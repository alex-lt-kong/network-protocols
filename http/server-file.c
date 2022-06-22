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

/**
 * @brief Get the file object
 * users are reminded to free() the buffer pointer themselves.
 * @param filename 
 * @return pointer to bytes of the specified file or NULL if error occurs.
 */
unsigned char* get_file(char* file_name, size_t* file_len) {
    char *file_buffer;
    FILE *file;
    //Open file
    file = fopen(file_name, "rb");
    if (!file) {
        perror("In fopen()");
        return NULL;
    }

    
    fseek(file, 0, SEEK_END);
    *file_len = ftell(file);
    fseek(file, 0, SEEK_SET);
    file_buffer=(unsigned char*)malloc(*file_len + 1);
    if (!file_buffer) {
        perror("In malloc()");        
        fclose(file);
        return NULL;
    }
    fread(file_buffer, *file_len, 1, file);
    fclose(file);
    return file_buffer;
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
    unsigned char* file_buffer;
    size_t resp_len = -1, file_len = -1;
    
    
    memset((char*)&address, 0, sizeof(address)); 
    address.sin_family = AF_INET;    
    address.sin_addr.s_addr = htonl(INADDR_ANY);     
    address.sin_port = htons(HTTP_PORT);
    

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("In bind()"); 
        return 0; 
    }

    if (listen(server_fd, 10) < 0) {
        perror("In listen()");
        exit(EXIT_FAILURE);
    }
    printf("[%s] Listening on 0.0.0.0:%d\n", get_iso_datetime(iso_dt), HTTP_PORT);
    int new_socket;
    while(1) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        printf("[%s] New connection accept()'ed\n", get_iso_datetime(iso_dt));
        if (new_socket < 0) {            
            perror("In accept()");
            free(resp);
            exit(EXIT_FAILURE);
        }
        
        char buffer[READ_BUF_SIZE] = {0};
        size_t bytes_read = -1;
        if (bytes_read = read(new_socket, buffer, READ_BUF_SIZE) != -1) {
            printf("========== Data received at [%s]: ==========\n%s\n---------- Data ends ----------\n", get_iso_datetime(iso_dt), buffer);
            file_buffer = get_file("server-file.mp4", &file_len);
    
            snprintf(header, HEADER_BUF_SIZE,
                "HTTP/1.1 200 OK\n"     
                "Server: Snakeoil/2.2.3\n"
                "Content-Type: video/mp4\n"
                "Content-Length: %d\n"
                "Accept-Ranges: bytes\n"
                "Connection: close\n"
                // Connection: closeIndicates that either the client or the server would like to close the connection.
                // This is the default on HTTP/1.0 requests. (On HTTP/1.1 requests, the default value is keep-alive.)
                "\n",
            file_len);
            resp = (unsigned char*)malloc(strlen(header) + file_len);
            strcpy(resp, header);
            memcpy(resp + strlen(header), file_buffer, file_len);
            resp_len = strlen(header) + file_len;
            free(file_buffer);
            if (write(new_socket, resp ,resp_len) != -1) {
                printf("Response sent\n");
            } else {
                perror("In write()");
            }
        } else {
            perror("In read()");
        }        
        close(new_socket);
        printf("[%s] Connection close()'ed gracefully\n\n", get_iso_datetime(iso_dt));
    }
    close(server_fd);
    return 0;
}