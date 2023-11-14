#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char* stringToHex(const char* str) {
    unsigned char *buffer = (unsigned char*)malloc(strlen(str));
    strcpy((char*)buffer, str);
    return buffer;
}

int main(int argc, char **argv) {
    unsigned char *buffer;
    buffer = stringToHex(argv[1]);
    int i;
    for (i = 0; buffer[i] != '\0'; i++) {
        printf("0X%02X ", buffer[i]);
    }
    printf("\n");
    free(buffer); 
    return 0;
}

