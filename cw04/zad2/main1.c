#define _POSIX_C_SOURCE 199309L

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>

int stopped = 0;
int total_copies = 0;
unsigned int lines = 0;
pid_t* child_pid = NULL;

unsigned int count_lines_in_file(char* path){
    FILE* to_read;
    unsigned int lines_number = 0;

    to_read = fopen(path, "r");
    while(!feof(to_read))
    {
        char ch = fgetc(to_read);
        if(ch == '\n')
        {
            lines_number++;
        }
    }
    fclose(to_read);
    return lines_number;
}


int read_whole_file(char** buffer, char* path){
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
    *buffer = malloc(file_size * sizeof(char));
    int r = fread(*buffer, file_size, 1, to_read);
    if(r != 1){
        fclose(to_read);
        printf("Unable to read data from file: %s\n", path);
        return -1;
    }
    fclose(to_read);
    return file_size;
}


int save_file(char* buffer, long bytes, char* filename, char* directory){
    char path[256];
    strcpy(path, directory);
    strcat(path, "/");
    strcat(path, filename);
    FILE* save_dest = fopen(path, "w");
    if(save_dest == NULL){
        printf("Cannot create file to save copy\n");
        return -1;
    }
    fwrite(buffer, sizeof(char), bytes, save_dest);
    fclose(save_dest);
    return 0;
}


void monitor_file(char* path, char* archives_directory, unsigned int seconds){
    unsigned int current_time = 0;
    char *buffer = NULL;
    long bytes = read_whole_file(&buffer, path);
    if (bytes < 0) {
        printf("%s%s\n", "Cannot read data from file: ", path);
        exit(-1);
    }
    struct stat file_info;
    stat(path, &file_info);
    unsigned long last_modified = file_info.st_mtime;
    total_copies = 0;

    while(1) {
        if(stopped == 1) continue;
        sleep(seconds);
        stat(path, &file_info);
        if(file_info.st_mtime != last_modified){
            char date[30];
            char filename[256];
            char* filename_pointer = strrchr(path, '/');
            if(filename_pointer == NULL) filename_pointer = path;
            strcpy(filename, filename_pointer);
            strftime(date, 30, "_%Y-%m-%d_%H-%M-%S", localtime(&file_info.st_mtime));
            strcat(filename, date);
            save_file(buffer, bytes, filename, archives_directory);
            bytes = read_whole_file(&buffer, path);
            last_modified = file_info.st_mtime;
            total_copies++;
        }
        current_time += seconds;
    }
    free(buffer);
    exit(total_copies);
}

void copy_file(char* path, char* archives_directory){
    struct stat file_info;
    stat(path, &file_info);
    char date[30];
    char filename[256];
    char* filename_pointer = strrchr(path, '/');
    if(filename_pointer == NULL) filename_pointer = path;
    strcpy(filename, filename_pointer);
    strftime(date, 30, "_%Y-%m-%d_%H-%M-%S", localtime(&file_info.st_mtime));
    strcat(filename, date);
    char command[256];
    strcpy(command, "");
    strcat(command, archives_directory);
    strcat(command, "/");
    strcat(command, filename);
    execlp("cp", "cp", path, command, (char*)0);
}

void monitor_file_cp(char* path, char* archives_directory, unsigned int seconds){
    unsigned int current_time = 0;
    FILE* tmp = fopen(path , "r");
    if(tmp == NULL){
        printf("%s%s\n", "File not found: ", path);
        fclose(tmp);
        exit(-1);
    }
    fclose(tmp);
    struct stat file_info;
    stat(path, &file_info);
    unsigned long last_modified = file_info.st_mtime;
    pid_t child_pid = fork();
    if(child_pid == 0){
        copy_file(path, archives_directory);
        exit(0);
    }
    total_copies = 1;

    while(1) {
        if(stopped == 1) continue;
        sleep(seconds);
        stat(path, &file_info);
        if(file_info.st_mtime != last_modified){
            pid_t child_pid = fork();
            if(child_pid == 0){
                copy_file(path, archives_directory);
                exit(0);
            }
            last_modified = file_info.st_mtime;
            total_copies++;
        }
        current_time += seconds;
    }
    exit(total_copies);
}


void catchSIGUSR1(int signo){
    if(stopped == 0){
        stopped = 1;
        pid_t pid = getpid();
        printf("\rProcess with PID %d has been stopped\n", pid);
    }
    else{
        pid_t pid = getpid();
        printf("\rProcess with PID %d is already stopped\n", pid);
    }
    signal(SIGUSR1, catchSIGUSR1);
}

void catchSIGUSR2(int signo){
    if(stopped == 1){
        stopped = 0;
        pid_t pid = getpid();
        printf("\rProcess with PID %d has been started\n", pid);
    }
    else{
        pid_t pid = getpid();
        printf("\rProcess with PID %d is not stopped\n", pid);
    }
    signal(SIGUSR2, catchSIGUSR2);
}

void catchSIGINT(int signo){
    exit(total_copies);
    signal(SIGINT, catchSIGINT);
}

void parent_catchSIGINT(int signo){
    for(int i = 0; i < lines; i++){
        kill(child_pid[i], SIGINT);
    }
    printf("\r");
    for(int i = 0; i < lines; i++){
        int result;
        waitpid(child_pid[i], &result, 0);
        printf("%s %d %s: %d\n", "PID", child_pid[i], "copies", WEXITSTATUS(result));
    }
    exit(0);
}


int main(int argc, char* argv[]){
    signal(SIGINT, parent_catchSIGINT);
    FILE* list;
    char path[256];
    char archives_directory[256];
    strcpy(archives_directory, "archiwum");
    unsigned int seconds;
    unsigned int mode;
    char file_list[256];
    if(argc != 3){
        printf("Incorrect number of parameters. Expected name of file with filenames to monitor (string) and mode (0 or 1)\n");
        return -1;
    }
    if(sscanf(argv[1], "%s", file_list) == EOF){
        printf("Incorrect first argument. Expected name of file with filenames to monitor\n");
        return -1;
    }
    if(sscanf(argv[2], "%d", &mode) == EOF){
        printf("Incorrect second argument. Expected 0 or 1 (monitor mode)\n");
        return -1;
    }
    if(mode > 1){
        printf("Incorrect third argument. Expected 0 or 1 (monitor mode)\n");
        return -1;
    }

    list = fopen("lista", "r");
    lines = count_lines_in_file("lista");
    child_pid = malloc(lines * sizeof(pid_t));
    int i = 0;

    while(fscanf(list,"%s %d", path, &seconds) > 0) {
        if(seconds <= 0){
            printf("Incorrect seconds number: %s %d\n", path, seconds);
            return -1;
        }
        child_pid[i++] = fork();
        if(child_pid[i - 1] == 0) {
            signal(SIGUSR1, catchSIGUSR1);
            signal(SIGUSR2, catchSIGUSR2);
            signal(SIGINT, catchSIGINT);
            if(mode == 0) monitor_file(path, archives_directory, seconds);
            else if(mode == 1) monitor_file_cp(path, archives_directory, seconds);
        }
        else{
            printf("Process with PID: %d is monitoring file: %s\n", child_pid[i - 1], path);
        }
    }

    int end = 0;
    while(end == 0){
        char command[256];
        scanf("%s", command);
        if(strcmp(command, "LIST") == 0){
            printf("PIDs of monitoring processes:\n");
            for(int i = 0; i < lines; i++){
                printf("%d\n", child_pid[i]);
            }
        }
        else if(strcmp(command, "STOP") == 0){
            int pid_to_stop;
            char stop_all[8];
            if(scanf("%d", &pid_to_stop) == 1){
                int found = 0;
                for(int i = 0; i < lines; i++){
                    if(child_pid[i] == pid_to_stop){
                        found = 1;
                        kill(pid_to_stop, SIGUSR1);
                        break;
                    }
                }
                if(found == 0){
                    printf("Cannot find process with PID: %d\n", pid_to_stop);
                }
            }
            else if(scanf("%s", stop_all) != EOF){
                if(strcmp(stop_all, "ALL") == 0){
                    for(int i = 0; i < lines; i++){
                        kill(child_pid[i], SIGUSR1);
                    }
                }
            }
        }
        else if(strcmp(command, "START") == 0){
            int pid_to_start;
            char start_all[8];
            if(scanf("%d", &pid_to_start) == 1){
                int found = 0;
                for(int i = 0; i < lines; i++){
                    if(child_pid[i] == pid_to_start){
                        found = 1;
                        kill(pid_to_start, SIGUSR2);
                        break;
                    }
                }
                if(found == 0){
                    printf("Cannot find process with PID: %d\n", pid_to_start);
                }
            }
            else if(scanf("%s", start_all) != EOF){
                if(strcmp(start_all, "ALL") == 0){
                    for(int i = 0; i < lines; i++){
                        kill(child_pid[i], SIGUSR2);
                    }
                }
            }
        }
        if(strcmp(command, "END") == 0){
            for(int i = 0; i < lines; i++){
                kill(child_pid[i], SIGINT);
            }
            end = 1;
        }
    }

    for(i = 0; i < lines; i++){
        int result;
        waitpid(child_pid[i], &result, 0);
        printf("%s %d %s: %d\n", "PID", child_pid[i], "copies", WEXITSTATUS(result));
    }


    free(child_pid);
    fclose(list);
    return 0;
}