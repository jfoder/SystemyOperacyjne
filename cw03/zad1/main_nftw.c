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
#include <ftw.h>

int sourcePathLength;
int compareOption;
time_t compareTime;

static int display(const char *path, const struct stat *buf, int tflag, struct FTW *ftwbuf){

    pid_t child_pid;
    child_pid = vfork();
    if(child_pid == 0){
        printf("\n%s", &path[sourcePathLength]);
        execl("/bin/ls", "ls", "-l", path, NULL);
        exit(0);
    }
    return 0;
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
    compareTime = t;
    if(argv[1][0] != '/') {
        sourcePathLength = strlen(cwd);
        nftw(cwd, display, 20, FTW_PHYS);
    }
    else {
        sourcePathLength = strlen(argv[1]);
        nftw(argv[1], display, 20, FTW_PHYS);
    }
    return 0;
}
  

