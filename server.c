// Name: Vivek Ponnala
// Date: October 26, 2020
// Title: Lab assignment 6: Stop and Wait for an Unreliable Channel, with Loss
// Description: This program is the client for a Stop and Wait reliable protocol built on top of UDP to provide a reliable transport service while considering loss

//Explain what each library does in the context of this lab. Do we need all of them?
#include <arpa/inet.h>      // used for the in_port_in and in_addr_t
#include <stdio.h>          // used for input and output
#include <stdlib.h>         // used for memory management, string conversion,
#include <string.h>         // used for strings 
#include <sys/socket.h>     // used for socklen_t and socket
#include <time.h>           // used for time and date
#include <unistd.h>         // used for constants and types
#include <fcntl.h>          // used for file control
			    // we need all the header files
#include "lab6.h"

int seq = 0;

//add the missing functions
int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    }
    return checksum;
}

void logPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
           packet.header.seq_ack,
           packet.header.len,
           packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

void ServerSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    // Simulatting a chance that ACK gets lost
    // if (rand() % PLOSTMSG == 0) {
    if (rand()%(10) + 1 == 0) {
        //******STUDENT WORK******
        printf("ACK lost\n");
        return;
    }
    //prepare and send the ACK
    Packet packet;
    //******STUDENT WORK******
    packet.header.seq_ack = seqnum;
    packet.header.len = 0;
    packet.header.cksum = getChecksum(packet);

    printf("Sent ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
}

Packet ServerReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
    Packet packet;

    while (1) {
//        Receive a packet from the client
        //******STUDENT WORK******
        printf("Server waiting for client\n");
        recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        logPacket(packet);

        // validate the length of the packet
        //******STUDENT WORK******
        int lengthOfPacket = strlen(packet.data) + 1;

        if (packet.header.len == 0) {
            lengthOfPacket = strlen(packet.data);
        }
        if (lengthOfPacket != packet.header.len) {
            printf("Length of packet does not match with packet header length");
        }

        // log what was received
        printf("Received: ");
        logPacket(packet);

        //verify the checksum and the sequence number
        if (packet.header.cksum != getChecksum(packet)) {
            printf("Bad checksum, expected %d\n", getChecksum(packet));
            //******STUDENT WORK******
            ServerSend(sockfd, address, *addrlen, !seq);
        } else if (packet.header.seq_ack != seq) {
            printf("Bad seqnum, expected %d\n", seq);
            //******STUDENT WORK******
            ServerSend(sockfd, address, *addrlen, !seq);
        } else {
            printf("Good packet\n");
            //******STUDENT WORK******
            ServerSend(sockfd, address, *addrlen, seq);
            seq != seq;
        }
        // }

        printf("\n");
        return packet;
    }
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create a socket
    //******STUDENT WORK******
    int sockfd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(struct sockaddr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // initialize the server address structure
    //******STUDENT WORK******

    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
    //******STUDENT WORK******
    if ((bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr))) == -1){
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    //Please answer the following questions
    //[Q1] AF_INET is an address family which provides the addresses the socket can communicate with
    //[Q2] SOCK_DGRAM is used for sending packets unreliably via UDP
    //[Q3] Htons stands for host to network short which converts values from host to network byte order.
    //[Q4] Client only require address of server. However, server establishes listening queue for clients, it needs bind function.
    //open files
    FILE *file = fopen(argv[2],"wb");
    if(file==NULL)
    {
	perror("fopen");
	return EXIT_FAILURE;
    }
    // get file contents from client and save it to the file
    int seqnum = 0;
    Packet packet;
    struct sockaddr_in clientaddr;
    char pktData[10];
    socklen_t clientaddr_len = sizeof(clientaddr);

    do {
        //******STUDENT WORK******
        packet = ServerReceive(sockfd, (struct sockaddr*)&clientaddr, &clientaddr_len, packet.header.seq_ack);
        printf("Packet header len: %d\n", packet.header.len);
        printf("Packet data: %s", packet.data);
        fwrite(packet.data, 1, packet.header.len, file);
	    seqnum = (seqnum+1)%2;
    } while (packet.header.len != 0);

    //cleanup
    //******STUDENT WORK******
    fclose(file);
    close(sockfd);
    
    return 0;
}
