// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
    
#define PORT     8080 
#define BUF_SIZE 65000 

// Driver code 
int main() { 
    int sockfd; 
    char buffer[READ_BUF_SIZE]; 
    char *hello = "Hello from server"; 
    struct sockaddr_in servaddr, cliaddr; 
        
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
        
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
        
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
        
    // Bind the socket with the server address 
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { 
        perror("In bind()"); 
        exit(EXIT_FAILURE); 
    } 
        
    int len;
    long long int n; 
    unsigned char int_buf[32];
    double send_ts, prev_send_ts = -1;
    len = sizeof(cliaddr);  //len is value/result 
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        n = recvfrom(sockfd, (char *)buffer, READ_BUF_SIZE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len); 
        buffer[n] = '\0'; 
        memset(int_buf, 0, sizeof(int_buf));
        memcpy(int_buf, buffer, strlen("1655712915.539565"));
        send_ts = atof(int_buf);
        printf("Message received, sent at %lf, length == %ld, \n", send_ts, n); 
        if (prev_send_ts > send_ts) {
            printf("===== Gotcha! Datagram out of order! =====\n");
            return 1;
        }
        prev_send_ts = send_ts;
    }
    return 0; 
}