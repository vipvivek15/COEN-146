/*
* Name: Vivek Ponnala
* Date: 10/8/2020
* Title: Lab 3 - Part 2
* Description: This is a UDP client program that sends/receives a file.
*/

#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(){
	//Declare socket file descriptor. All Unix I/O streams are referenced by descriptors
	int sockfd;

    // Declare sending buffer of size 1k bytes
    char sbuf[1024];

    // Declare server address
    struct sockaddr_in servAddr;

    // Convert domain names into numerical IP addresses via DNS
    struct hostent *host;
    host = (struct hostent *)gethostbyname("localhost"); //Local host runs the server. You may use "127.0.0.1": loopback IP address
    
    FILE *fptr;
    fptr = fopen("input.txt", "rb");
    if (fptr == NULL) {
        printf("Cannot open file\n");
        exit(0);
    }
    
    //Open socket, if successful returns socket with endpoint
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // Set the server address to send using socket addressing structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5000);
    servAddr.sin_addr = *((struct in_addr *)host->h_addr);

    // Client to send messages to the server continuously using UDP socket
    
    while(1){
        printf("Client: Type a message to send to Server:\n");
        scanf("%s", sbuf);
        sendto(sockfd, sbuf, strlen(sbuf), 0, (struct sockaddr *)&servAddr, sizeof(struct sockaddr));
    }

    // send data contents of source file
    
    while (!feof(fptr)) {
        int character = fread(sbuf, 1, sizeof(sbuf), fptr);
        //send to server
        sendto(sockfd, sbuf, character, 0, (struct sockaddr *)&servAddr, sizeof(struct sockaddr));
    }

    return 0;
}
