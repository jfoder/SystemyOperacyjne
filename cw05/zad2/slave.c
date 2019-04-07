#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>


int main(int argc, char* argv[]) {
    srand(time(NULL));
    if(argc != 3) {
        printf("Incorrect number of arguments. Expected string (fifo name) and integer (number of lines to send)\n");
        return -1;
    }
    int n;
    if(sscanf(argv[2], "%d", &n) != 1) {
        printf("Incorrect second argument of slave program. Expected positive integer (number of lines to send)\n");
        return -1;
    }
    if(n <= 0) {
        printf("Incorrect second argument of slave program. Expected positive integer (number of lines to send)\n");
        return -1;
    }

    pid_t my_pid = getpid();
    printf("PID of slave: %d\n", my_pid);
    int fifo = open(argv[1], O_WRONLY);
    if(fifo == -1) {
        printf("Cannot open selected fifo\n");
        return -1;
    }

    for(int i = 0; i < n; i++) {
        char output[256];
        FILE* date_file = popen("date", "r");
        fread(output, sizeof(char), 256, date_file);
        pclose(date_file);
        char final_output[512];
        sprintf(final_output, "PID: %d %s", my_pid, output);
        printf("%s", final_output);
        write(fifo, final_output, strlen(final_output));
        sleep(rand() % 4 + 2);
    }
    close(fifo);
    return 0;
}



