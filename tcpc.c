/*
 * Name: Vivek Ponnala
 * Date: 10/14/2020
 * Title: Lab 4 - Part 1
 * Description: This program is implemented as the TCP Client
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
int main()
{
// Declare socket file descriptor. All Unix streams referenced by descriptors. 
      int sockfd, nr;
// Declare sending and receivng buffers
      char sbuf[1024], rbuf[1024];
// Declare server address to connect to
    struct sockaddr_in servAddr;
    struct hostent *host;
	
//Convert domain names into numerical IP addresses
	 host = (struct hostent *)gethostbyname("localhost");   

//Open a socket and connect to server  
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // Set the server address to send using socket addressing structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5001);
    servAddr.sin_addr = *((struct in_addr *)host->h_addr);

    // Open a socket and connect to the server
    if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))){
        perror("Failure to connect to the server");
        exit(1);
    }

    FILE *fptr1;
    fptr1 = fopen("sample1.txt", "r");
    if (fptr1 == NULL){
        printf("Cannot open file\n");
        exit(0);
    }

    while (!feof(fptr1)) {
        int character = fread(sbuf, 1, sizeof(sbuf), fptr1);
        //send to server
        sendto(sockfd, sbuf, character, 0, (struct sockaddr *)&servAddr, sizeof(struct sockaddr));
    }


    // Client begins to write and read from the server
    /*while(1){
        printf("Client: Type a message to send to Server\n");
        scanf("%s", sbuf);
        write(sockfd, sbuf, strlen(sbuf));
        read(sockfd, rbuf, 1024);
        printf("Server: sent message: %s\n", rbuf);
    }*/
    // Close socket descriptor
    close(sockfd);
    return 0;
}
