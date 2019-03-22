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


void monitor_file(char* path, char* archives_directory, unsigned int seconds, unsigned int total_time){
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
    int total_copies = 0;

    while(current_time + seconds <= total_time) {
        sleep(seconds);
        stat(path, &file_info);
        if(file_info.st_mtime != last_modified){
            char date[30];
            char filename[256];
            char* filename_pointer = strrchr(path, '/');
            if(filename_pointer == NULL) filename_pointer = path;
            strcpy(filename, filename_pointer);
            printf(filename);
            strftime(date, 30, "_%Y-%m-%d_%H-%M-%S", localtime(&file_info.st_mtime));
            strcat(filename, date);
            printf("File modified: %s\n", filename);
            save_file(buffer, bytes, filename, archives_directory);
            bytes = read_whole_file(&buffer, path);
            last_modified = file_info.st_mtime;
            total_copies++;
        }
        current_time += seconds;
    }
    free(buffer);
    exit(0);
}


int main(int argc, char* argv[]){
    FILE* list;
    char path[256];
    char archives_directory[256];
    strcpy(archives_directory, "archiwum");
    unsigned int seconds;
    unsigned int total_time = 15;
    int mode = 0;

    list = fopen("lista", "r");
    unsigned int lines = count_lines_in_file("lista");
    pid_t* child_pid = malloc(lines * sizeof(pid_t));
    int i = 0;

    while(fscanf(list,"%s %d", path, &seconds) > 0) {
        child_pid[i++] = fork();
        if(child_pid[i - 1] == 0) {
            if(mode == 0) monitor_file(path, archives_directory, seconds, total_time);
        }
    }
    free(child_pid);
    fclose(list);
    return 0;
}
  

