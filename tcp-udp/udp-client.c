// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <time.h>
#include <string.h> 
#include <sys/types.h> 
#include <sys/time.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
    
#define PORT     8080 
#define BUF_SIZE 65000 
#define MSG_COUNT 10000000

// Driver code 
int main() { 
    struct timeval tp;
    int socket_fd; 
    char msg[READ_BUF_SIZE]; 
    struct sockaddr_in     servaddr; 
    int msg_padding_lens[] = {0, 1024, 32767, 64500};
    srand(time(NULL));

    // Creating socket file descriptor 
    if ( (socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    memset(&servaddr, 0, sizeof(servaddr)); 
        
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
        
    int msg_len = -1; 
    FILE *f = fopen("stdout.txt", "w");
    if (f == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }


    for (int i = 0; i < MSG_COUNT; ++i) {
        gettimeofday(&tp, NULL);
        memset(msg, 0, sizeof(msg));
        snprintf(msg, READ_BUF_SIZE, "%d.%06d: msg from client", tp.tv_sec, tp.tv_usec);
        msg_len = strlen("1655714250.706089: msg from client");
        memset(msg + msg_len, 'a', msg_padding_lens[rand() % sizeof(msg_padding_lens) / sizeof(int)]);
        sendto(socket_fd, msg, strlen(msg), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
        //printf("[%d/%d] Message sent at %d.%06d, length == %d\n", i+1, MSG_COUNT, tp.tv_sec, tp.tv_usec, strlen(msg));
        fprintf(f, "[%d/%d] Message sent at %d.%06d, length == %d\n", i+1, MSG_COUNT, tp.tv_sec, tp.tv_usec, strlen(msg));
    }
    fclose(f);
    
    close(socket_fd); 
    return 0; 
}