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


int compareOption;
time_t compareTime;

static int display(const char *path, const struct stat *buf, int tflag, struct FTW *ftwbuf){
    //printf("%s\n", &(path[ftwbuf->base]));
    if(strcmp(&(path[ftwbuf->base]), "..") == 0 || strcmp(&(path[ftwbuf->base]), ".") == 0) return 0;
    if(compareOption == 0){
        char date1[20];
        char date2[20];
        strftime(date1, 20, "%Y-%m-%d", localtime(&buf->st_mtime));
        strftime(date2, 20, "%Y-%m-%d", localtime(&compareTime));
        if(strcmp(date1, date2) != 0) return 0;
    }
    else if(compareOption == 1){
        if(buf->st_mtime >= compareTime) return 0;
        char date1[20];
        char date2[20];
        strftime(date1, 20, "%Y-%m-%d", localtime(&buf->st_mtime));
        strftime(date2, 20, "%Y-%m-%d", localtime(&compareTime));
        if(strcmp(date1, date2) == 0) return 0;
    }
    else if(compareOption == 2){
        if(buf->st_mtime <= compareTime) return 0;
        char date1[20];
        char date2[20];
        strftime(date1, 20, "%Y-%m-%d", localtime(&buf->st_mtime));
        strftime(date2, 20, "%Y-%m-%d", localtime(&compareTime));
        if(strcmp(date1, date2) == 0) return 0;
    }
    printf("%s ", path);
    if(S_ISREG(buf->st_mode) == 1) printf("FILE ");
    else if(S_ISDIR(buf->st_mode) == 1) printf("DIR ");
    else if(S_ISCHR(buf->st_mode) == 1) printf("CHAR DEV ");
    else if(S_ISBLK(buf->st_mode) == 1) printf("BLOCK DEV ");
    else if(S_ISFIFO(buf->st_mode) == 1) printf("FIFO ");
    else if(S_ISLNK(buf->st_mode) == 1) printf("SLINK ");

    printf("%ldB ", buf->st_size);
    char date[20];
    strftime(date, 20, "%Y-%m-%d", localtime(&buf->st_atime));
    printf("ACCESS: %s ", date);
    strftime(date, 20, "%Y-%m-%d", localtime(&buf->st_mtime));
    printf("MOD: %s\n", date);
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
    if(argv[1][0] != '/') nftw(cwd, display, 20, FTW_PHYS);
    else nftw(argv[1], display, 20, FTW_PHYS);
    return 0;
}
  

