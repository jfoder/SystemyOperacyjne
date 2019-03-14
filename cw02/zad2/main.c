#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>


void printFiles(char* directoryName){
    DIR* dir = opendir(directoryName);
    struct dirent* d;
    rewinddir(dir);
    char cwd[256];
    strcpy(cwd, directoryName);
    while((d = readdir(dir)) != NULL){
        struct stat buf;
        char path[256];
        strcpy(path, cwd);
        strcat(path, "/");
        strcat(path, d->d_name);
        stat(path, &buf);
        printf("%s ", path);
        if(S_ISREG(buf.st_mode) == 1) printf("REG FILE ");
        else if(S_ISDIR(buf.st_mode) == 1) printf("DIRECTORY");

        printf("\n");
    }
    rewinddir(dir);
    while((d = readdir(dir)) != NULL){
        struct stat buf;
        char path[256];
        strcpy(path, cwd);
        strcat(path, "/");
        strcat(path, d->d_name);
        stat(path, &buf);
        if(strcmp(d->d_name, "..") != 0 && strcmp(d->d_name, ".") != 0 && S_ISDIR(buf.st_mode) == 1){
            printFiles(path);
        }
    }

}



int main(int argc, char* argv[]){
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/");
    strcat(cwd, argv[1]);
    if(argv[1][0] != '/') printFiles(cwd);
    else printFiles(argv[1]);
    return 0;
}
  

