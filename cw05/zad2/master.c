#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Incorrect number of arguments. Expected one string (fifo name)\n");
        return -1;
    }
    mkfifo(argv[1], S_IWRITE | S_IREAD);
    char input[512];
    FILE* fifo = fopen(argv[1], "r");
    if(fifo == NULL) {
        printf("Cannot open created fifo\n");
        return -1;
    }
    while(fgets(input, 512, fifo) != NULL) {
        printf("%s\n", input);
    }
    return 0;
}



