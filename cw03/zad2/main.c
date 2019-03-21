#define _POSIX_C_SOURCE 199309L

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>


int read_whole_file(char* buffer, char* path){
    FILE *to_read;
    long file_size;

    to_read = fopen(path , "rb");
    if(to_read == NULL){
        printf("Unable to open file: %s\n", path);
        return -1;
    }

    fseek(to_read, 0, SEEK_END);
    file_size = ftell(to_read);
    rewind(to_read);
    buffer = malloc(file_size * sizeof(char));

    if(fread(buffer, file_size, 1, to_read) != 1){
        fclose(to_read);
        printf("Unable to read data from file: %s\n", path);
        return -1;
    }
    fclose(to_read);
    return 0;
}


int main(int argc, char* argv[]){
    FILE* list;
    char path[256];
    unsigned int seconds;

    list = fopen("lista", "r");

    while(fscanf(list,"%s %d", path, &seconds) > 0) {
        printf("%s %d\n", path, seconds);
        pid_t child_pid = fork();
        if(child_pid == 0){
            sleep(seconds);
            char* buffer = NULL;
            if(read_whole_file(buffer, path) == 0){
                printf("%s\n", buffer);
            }
            exit(0);

        }
    }
    fclose(list);


    for(int i = 1; i <= 10; i++){
        pid_t child_pid;
        child_pid = fork();
        if(child_pid == 0){
            sleep(i);
            printf("%s: %d\n", "Child process", i);
            exit(0);
        }
    }
    return 0;
}
  

