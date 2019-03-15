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


static int display(const char *path, const struct stat *buf, int tflag, struct FTW *ftwbuf){
    printf("%s\n", &(path[ftwbuf->base]));
    return 0;
}


void printFiles(char* directoryName, int compareOption, time_t time){
    DIR* dir = opendir(directoryName);
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
        stat(path, &buf);
        if(strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, ".") == 0) continue;
        if(compareOption == 0){
            char date1[20];
            char date2[20];
            strftime(date1, 20, "%Y-%m-%d", localtime(&buf.st_mtime));
            strftime(date2, 20, "%Y-%m-%d", localtime(&time));
            if(strcmp(date1, date2) != 0) continue;
        }
        else if(compareOption == 1){
            if(buf.st_mtime >= time) continue;
            char date1[20];
            char date2[20];
            strftime(date1, 20, "%Y-%m-%d", localtime(&buf.st_mtime));
            strftime(date2, 20, "%Y-%m-%d", localtime(&time));
            if(strcmp(date1, date2) == 0) continue;
        }
        else if(compareOption == 2){
            if(buf.st_mtime <= time) continue;
            char date1[20];
            char date2[20];
            strftime(date1, 20, "%Y-%m-%d", localtime(&buf.st_mtime));
            strftime(date2, 20, "%Y-%m-%d", localtime(&time));
            if(strcmp(date1, date2) == 0) continue;
        }
        printf("%s ", path);
        if(S_ISREG(buf.st_mode) == 1) printf("FILE ");
        else if(S_ISDIR(buf.st_mode) == 1) printf("DIR ");
        else if(S_ISCHR(buf.st_mode) == 1) printf("CHAR DEV ");
        else if(S_ISBLK(buf.st_mode) == 1) printf("BLOCK DEV ");
        else if(S_ISFIFO(buf.st_mode) == 1) printf("FIFO ");
        else if(S_ISLNK(buf.st_mode) == 1) printf("SLINK ");

        printf("%ldB ", buf.st_size);
        char date[20];
        strftime(date, 20, "%Y-%m-%d", localtime(&buf.st_atime));
        printf("ACCESS: %s ", date);
        strftime(date, 20, "%Y-%m-%d", localtime(&buf.st_mtime));
        printf("MOD: %s\n", date);
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
    if(argv[1][0] != '/') nftw(cwd, display, 20, 0);
    else nftw(argv[1], display, 20, 0);
    return 0;
}
  

