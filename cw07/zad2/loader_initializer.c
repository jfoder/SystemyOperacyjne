#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include "header.h"


#define MAX_LOADERS 1024

int loaders_pids[MAX_LOADERS];
int loaders;


void sigint_handler(int signo) {
    for(int i = 0; i < loaders; i++) {
        kill(loaders_pids[i], SIGINT);
    }
    exit(0);
}


int main(int argc, char *argv[]) {
    struct sigaction sigact;
    sigact.sa_handler = sigint_handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, NULL);

    if(argc != 3 && argc != 4) {
        printf("[INITIALIZER] Wrong number of arguments, expected number of loaders, line weight limit and (optionally) number of packages for each loader\n");
        exit(-1);
    }
    int weight_limit, iterations = -1;
    if(sscanf(argv[1], "%d", &loaders) != 1) {
        printf("[INITIALIZER] Wrong first argument, expected number of loaders (int)\n");
        exit(-1);
    }
    if(sscanf(argv[2], "%d", &weight_limit) != 1) {
        printf("[INITIALIZER] Wrong second argument, expected line weight limit (int)\n");
        exit(-1);
    }
    if(argc == 4 && sscanf(argv[3], "%d", &iterations) != 1) {
        printf("[INITIALIZER] Wrong third argument, expected number of packages for each loader (int)\n");
        exit(-1);
    }
    char first_argument[8];
    char second_argument[8];
    srand(time(NULL));
    sprintf(first_argument, "%d", (rand() % weight_limit) + 1);
    if(argc == 4) {
        sprintf(second_argument, "%d", iterations);
    }

    for(int i = 0; i < loaders; i++) {
        int rand_result = (rand() % weight_limit) + 1;
        loaders_pids[i] = fork();
        if(loaders_pids[i] == 0) {
            printf("RAND: %d\n", rand_result);
            sprintf(first_argument, "%d", rand_result);
            if(argc == 3) {
                execl("./loader", "loader", first_argument, NULL);
                return 0;
            }
            else if(argc == 4) {
                execl("./loader", "loader", first_argument, second_argument, NULL);
                return 0;
            }
        }
        else {
            printf("Process launched\n");
        }
    }

    return 0;
}