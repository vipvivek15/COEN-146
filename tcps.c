/*
 * Name: Vivek Ponnala
 * Date: 10/14/2020
 * Title: Lab 4 - Part 1
 * Description: This program is implemented as the TCP Server
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define NCONNS 5

FILE *fileAddress;
void *start_routine(void *arg);
pthread_t tids[NCONNS];
int main()
{ 
   //Declare socket file descriptor
    int sockfd, connfd, rb, sin_size;

    // Declare receiving and sending buffers of size 1k bytes
    char rbuf[1024], sbuf[1024];

    // Declare server address to which to bind for receiving messages and client address to fill in sending address
    struct sockaddr_in servAddr, clienAddr;
  
 // Open a TCP socket, if successful return a descriptor
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // Setup the server address to bind using socket addressing structure
    servAddr.sin_family = AF_INET; // Internet protocol
    servAddr.sin_port = htons(5001); // Address port
    servAddr.sin_addr.s_addr = INADDR_ANY; // Internet address

    // Set address/port of server endpoint for socket socket descriptor
    if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0){
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    printf("Server waiting for client\n");

    if (listen(sockfd, NCONNS) == -1) { // return -1 if connection is unsuccessful
        perror("Listen");
        exit(1);
    }
    int i;
    for(i = 0; i <= NCONNS ; i++){
        int *CONN = malloc(sizeof(int));
        *CONN = accept(sockfd, (struct sockaddr *)&clienAddr, (socklen_t *)&sin_size);
        pthread_create(&tids[i], NULL, start_routine, CONN);
    }
    fclose(fileAddress);
    close(sockfd);
    return 0;
}

void *start_routine(void *arg){
    char recv_data[1024];
    int bytes_received;
    int connfd = *(int*)arg;
    printf("%d\n", connfd);
    while((bytes_received = recv(connfd, recv_data, 1024, 0)) > 0) {
        // Open global file for writing
        fileAddress = fopen("out.txt", "a");

        if (!feof(fileAddress)){
            fwrite(recv_data,1, bytes_received, fileAddress);
        }
        else {
            printf("\n Unable to Create or Open the sample1.txt File");
        }
        fflush(fileAddress);  //changed to fflush
    }
    printf("Received data: %s", recv_data);
}
