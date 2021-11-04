/*
* Name: Vivek Ponnala
* Date: 10/8/2020
* Title: Lab 3 - Part 2
* Description: This is a UDP server program that sends/receives a file.
*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(){
    int sockfd; // file descriptor of type int

    //Open a UDP socket, if successful return descriptor associated with endpoint
     if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    //Setup the server address to bind using socket addressing structure
    struct sockaddr_in servAddr, clienAddr; // serAddr: server address, clienAddr: client address
    socklen_t addrLen = sizeof(struct sockaddr);
    servAddr.sin_family = AF_INET; // IPv4
    servAddr.sin_port = htons(5000); //Port 5000 is assigned to server
    servAddr.sin_addr.s_addr = INADDR_ANY; //Local IP address of any interface is assigned

    //Set address/port of server endpoint for socket socket descriptor
    if (bind(sockfd,(struct sockaddr *)&servAddr, addrLen) == -1) {
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    // Open another file for writing
    
    FILE *fileAddress;
    fileAddress = fopen("out.txt", "wb");

    //Server continuously waits for messages from client, then prints incoming messages
    while (1) {
        printf("Server waiting for messages from client: \n");
        char recv_data[1024];
        int bytes_read = recvfrom(sockfd, recv_data, 1024, 0, (struct sockaddr *)&clienAddr, &addrLen);
        recv_data[bytes_read] = '\0';
        printf("Client with IP: %s and Port: %d sent message: %s\n", inet_ntoa(clienAddr.sin_addr),ntohs(clienAddr.sin_port), recv_data);
        fwrite(recv_data, 1, sizeof(recv_data), fileAddress);
        
    }
    return 0;
}

