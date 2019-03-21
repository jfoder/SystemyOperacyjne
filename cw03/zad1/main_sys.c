#define __USE_XOPEN
#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <wait.h>

int sourcePathLength;

void printFiles(char* directoryName, int compareOption, time_t time){
    DIR* dir = opendir(directoryName);

    pid_t child_pid;
    child_pid = vfork();
    if(child_pid == 0){
        printf("\n%s", &directoryName[sourcePathLength]);
        execl("/bin/ls", "ls", "-l", directoryName, NULL);
        exit(0);
    }

    if(dir == NULL){
        printf("Cannot open directory: %s\n", directoryName);
        return;
    }
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
        lstat(path, &buf);
        if(strcmp(d->d_name, "..") != 0 && strcmp(d->d_name, ".") != 0 && S_ISDIR(buf.st_mode)){
            printFiles(path, compareOption, time);
        }
    }

}



int main(int argc, char* argv[]){
    if(argc != 4){
        printf("Incorrect arguments number, expected three arguments in main\n");
        return -1;
    }
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/");
    strcat(cwd, argv[1]);
    char inputOption;
    int compareOption;
    if(sscanf(argv[2], "%c", &inputOption) == EOF){
        printf("Incorrect second argument in main, expected character (=, <, >)\n");
        return -1;
    }
    if(inputOption == '=') compareOption = 0;
    else if(inputOption == '<') compareOption = 1;
    else if(inputOption == '>') compareOption = 2;
    else{
        printf("Incorrect second argument in main, expected character (=, <, >)\n");
        return -1;
    }
    char date[16];
    if(sscanf(argv[3], "%s", date) == EOF){
        printf("Incorrect third argument in main, expected date (YYYY-MM-DD)\n");
        return -1;
    }

    struct tm tm = {0};
    strptime(date, "%Y-%m-%d", &tm);
    time_t t = mktime(&tm);
    char date1[20];
    strftime(date1, 20, "%Y-%m-%d", localtime(&t));
    if(argv[1][0] != '/') {
        sourcePathLength = strlen(cwd);
        printFiles(cwd, compareOption, t);
    }
    else {
        sourcePathLength = strlen(argv[1]);
        printFiles(argv[1], compareOption, t);
    }
    return 0;
}
  

