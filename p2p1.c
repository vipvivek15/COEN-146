//Name: Vivek Ponnala
//Date: 10/14/2020
/*
 * Title: Lab 4 - Step 2 - UDP Peer-to-Peer File Transfer
 * Description: Opens a socket on the localhost as the server or
 * on a remote host as the client and then when the server
 * receives a message from the client requesting the file
 * it transfers the file over. Then they can switch to send a file back
 * if desired. Or continue sending more files this way.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//Declare socket file descriptor. All Unix I/O streams are referenced by descriptors
int sockfd;

//Declare receiving buffer of size 1k bytes
char rbuf[1024], sbuf[1024];

//Declare server address to which to bind for receiving messages and client address to fill in sending address
struct sockaddr_in servAddr, remoteAddr;
socklen_t addrLen = sizeof(struct sockaddr);

struct hostent *host;

char host_name[20], source_file[20], dest_file[20];

void resolve_hostname_open_socket() {
    //Converts domain names into numerical IP addresses via DNS
    host = (struct hostent *)gethostbyname(host_name);

    if (host == NULL) {
        puts("Error: unable to resolve hostname.");
        exit(1);
    }

    //Setup the server address to bind using socket addressing structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5000); //Port 5000 is assigned
    //servAddr.sin_addr.s_addr = INADDR_ANY; //Local IP address of any interface is assigned (generally one interface IP address)
    servAddr.sin_addr = *((struct in_addr *)host->h_addr);

    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(6000); //Port 6000 is assigned
    //remoteAddr.sin_addr.s_addr = INADDR_ANY; //Local IP address of any interface is assigned (generally one interface IP address)
    remoteAddr.sin_addr = *((struct in_addr *)host->h_addr);

    //Open a UDP socket, if successful, returns a descriptor associated with an endpoint
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    /*int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }*/
}

void server() {
    gethostname(host_name, 20);
    printf("Server hostname: %s\n", host_name);

    resolve_hostname_open_socket();

    //Set address/port of server endpoint for socket socket descriptor
    if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0){
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    // wait for client to request a file
    int rb;
    while ((rb = recvfrom(sockfd, rbuf, 1024, 0, (struct sockaddr *)&remoteAddr, &addrLen)) > 0){
        rbuf[rb] = '\0';
        printf("Client sent: %s\n", rbuf);

        if (strcmp(rbuf, "REQUEST") == 0) {
            break;
        }
    }

    //Server opens the source file
    int fp_r = open(source_file, O_RDWR);
    if (fp_r < 0) {
        puts("Error opening source file!");
        sendto(sockfd, "ERROR", strlen("ERROR"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
        exit(1);
    }

    sendto(sockfd, "SENDING", strlen("SENDING"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));

    //Server reads in the source file and sends it in chunks of size sbuf size to the client
    size_t bytes;
    int file_bytes = 0;
    while (0 < (bytes = read(fp_r, sbuf, sizeof(sbuf)))) {
        sendto(sockfd, sbuf, bytes, 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
    }

    sendto(sockfd, "EOF", sizeof("EOF"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));

    // Close the file
    close(fp_r);
    puts("Successfully sent whole file to client. You deserve a cookie!");

    // Close the socket descriptor
    close(sockfd);
}

void client() {
    puts("Please enter the server hostname you'd like to receive from:");
    scanf("%s", host_name);

    resolve_hostname_open_socket();

    // Client sends file request
    sendto(sockfd, "REQUEST", strlen("REQUEST"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
    printf("Sent file request to server.\n");

    // wait for response
    int rb;
    while ((rb = recvfrom(sockfd, rbuf, 1024, 0, (struct sockaddr *)&remoteAddr, &addrLen)) > 0){
        rbuf[rb] = '\0';
        printf("Server sent: %s\n", rbuf);

        if (strcmp(rbuf, "SENDING") == 0) {
            break;
        }
        else if (strcmp(rbuf, "ERROR") == 0) {
            puts("Remote server could not open file. GG!");
            exit(1);
        }
    }

    /* Client opens the destination file:
     * O_CREAT = If pathname does not exist, create it as a regular file
     * O_RDWR = Open for reading and writing
     * 0777 = Octal value that represents access granted to users on the system
     */
    int fp_w = open(dest_file, O_RDWR | O_CREAT, 0777);
    if (fp_w < 0) {
        puts("Error opening the destination file! Might wanna try that again...");
        exit(1);
    }

    // client receives from the server and writes to the output file
    int file_bytes = 0;
    while ((rb = recvfrom(sockfd, rbuf, 1024, 0, (struct sockaddr *)&remoteAddr, &addrLen)) > 0){
        rbuf[rb] = '\0';
        if (strcmp(rbuf, "EOF") == 0) {
            puts("EOF reached!");
            break;
        }
        else {
            write(fp_w, rbuf, strlen(rbuf));
        }
    }

    // close the file
    close(fp_w);
    puts("File was successfully written to. Give yourself a pat on the back!");

    // Close socket descriptor
    close(sockfd);
}

int main() {
    while (1) {
        puts("Hello! Would you like to send (0), receive (1), or exit (2)?");
        int input;
        scanf("%d", &input);
        if (input == 0) {
            puts("Please enter the name of the file that you'd like to send:");
            scanf("%s", source_file);
            server();
        }
        else if (input == 1) {
            puts("Please enter the name of the file that you'd like to write to:");
            scanf("%s", dest_file);
            client();
        }
        else if (input == 2) {
            puts("Alright sounds good. Have a nice day!");
            break;
        }
        else {
            puts("Sorry, I didn't understand that. Please try again!");
        }
    }
    return 0;
}
