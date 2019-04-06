#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <memory.h>
#include <ctype.h>


char** get_args(char* command) {
    char *end;
    while(isspace((unsigned char) *command)) ++command;
    if(*command == 0) {
        printf("Incorrect command\n");
        exit(-1);
    }
    end = command + strlen(command) - 1;
    while(end > command && isspace((unsigned char) *end)) --end;
    end[1] = '\0';
    int size = 0;
    char** command_arguments = NULL;
    char* buffer = strtok(command, " ");
    while(buffer != NULL) {
        size++;
        command_arguments = realloc(command_arguments, sizeof(char*) * size);
        command_arguments[size - 1] = buffer;
        buffer = strtok(NULL, " ");
    }
    command_arguments = realloc(command_arguments, sizeof(char*) * (size + 1));
    command_arguments[size] = NULL;
    return command_arguments;
}


void execute_command(char* command_line) {
    int pipes[2][2];
    char* commands[32];
    unsigned int command_index = 0;
    char* buffer = strtok(command_line, "|");
    while(buffer != NULL) {
        commands[command_index++] = buffer;
        buffer = strtok(NULL, "|");
    }
    int i;
    for(i = 0; i < command_index; i++) {
        char** command_arguments = get_args(commands[i]);
        if(i > 0) {
            close(pipes[i % 2][0]);
            close(pipes[i % 2][1]);
        }
        if(pipe(pipes[i % 2]) != 0) {
            printf("Cannot create pipe\n");
            exit(-1);
        }
        pid_t single_command_pid = fork();
        if(single_command_pid == 0) {
            if(i < command_index - 1) {
                close(pipes[i % 2][0]);
                if (dup2(pipes[i % 2][1], STDOUT_FILENO) < 0) {
                    printf("Cannot replace standard input descriptor\n");
                    exit(-1);
                };
            }
            if(i > 0) {
                close(pipes[(i + 1) % 2][1]);
                if (dup2(pipes[(i + 1) % 2][0], STDIN_FILENO) < 0) {
                    printf("Cannot replace standard output descriptor\n");
                    exit(-1);
                }
            }
            execvp(command_arguments[0], command_arguments);
            exit(0);
        }
    }
    close(pipes[i % 2][0]);
    close(pipes[i % 2][1]);
    wait(NULL);
}


int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Incorrect number of arguments. Expected one string (filename with commands)\n");
        return -1;
    }
    FILE* command_file = fopen(argv[1], "r");
    if(command_file == NULL) {
        printf("Cannot open file with name specified as main parameter\n");
        return -1;
    }
    char line[512];
    while(fgets(line, 512, command_file)) {
        execute_command(line);
    }
    fclose(command_file);
    return 0;
}



