/*
* Name: Vivek Ponnala
* Date: October 6, 2020
* Title: Lab 3 - Part 1
* Description: This program is able to copy a binary/text file to another file by using function calls.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>


int main(){
    FILE *source, *dest;
    char filename[100], c;

    // first file is to read in its contents
    source = fopen("Lamborghini.jpeg", "rb");
    if (source == NULL) {
        puts("Cannot open file");
        exit(0);
    }

    fseek(source, 0, SEEK_END);

    // length = EOF for input file
    int length = ftell(source);

    fseek(source, 0, SEEK_SET);

    // second file will be written to
    dest = fopen("output.jpeg", "wb");
    if (dest == NULL) {
        puts("Cannot open file");
        exit(0);
    }

    // read contents from file  by parsing through each character of the input file
    int i;
    for(i = 0; i < length; i++){
        fputc(fgetc(source), dest);
    }

    printf("\nContents are now successfully copied to new file. You deserve a cookie");

    fclose(source);
    fclose(dest);
    return 0;
}
