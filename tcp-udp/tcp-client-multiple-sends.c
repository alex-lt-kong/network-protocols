// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "common.h"

int main(int argc, char const *argv[])
{
    struct timeval tp;
    int sock = 0;
    
    struct sockaddr_in serv_addr;
    char msg[64];// = "msg from client at %ld";
    char buffer[READ_BUF_SIZE] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("In connect()");
        return -1;
    }
    long bytes = -1;
    for (int i = 0; i < 10; ++i) {
        gettimeofday(&tp, NULL);
        snprintf(msg, sizeof(msg)-1, "msg from client at %d.%d", tp.tv_sec, tp.tv_usec);
        if (send(sock , msg , strlen(msg), 0) != -1) {
            // use send() instead of write() and use MSG_MORE to send messages in batches.
            // This, however, doesn't make much difference since we can accumulate the message and them
            // call send() once...
            bytes = read(sock, buffer, READ_BUF_SIZE);
            sleep(1);
            printf("Message [%s] sent and reply [%s] received\n", msg, buffer);
        } else {
            perror("In send():");
        }
    }
    close(sock);
    return 0;
}