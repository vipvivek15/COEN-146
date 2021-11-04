/*
* Name: Vivek Ponnala
* Date: October 6 2020
* Title: Lab 3 - Part 1
* Description: This program is able to copy a binary/text file to another file by using system calls.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>


int main(){
    char buf;

    // first file is to read in its contents
    int source = open("input.txt", O_RDWR);
    if (source == -1) {
        puts("Cannot open file");
        exit(0);
    }

    // create a file if one doesn't exist or prepare to write to it
    int dest = open("output.txt", O_WRONLY | O_CREAT, 0641);
    if(dest == -1) {
        perror("Destination file error");
        exit(0);
    }

    // write to the file until we reach the end of source file
    while((read(source, &buf, 1)) > 0) {
        write(dest, &buf, 1 );
    }
    printf("\nContents are now successfully copied to new file. You deserve a cookie");
    close(source);
    close(dest);

    return 0;
}
