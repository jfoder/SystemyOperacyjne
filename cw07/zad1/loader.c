#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include "header.h"


int semaphoreID;
int memoryID;
struct PackageQueue* queue;
int using_semaphore = 0;
int total_packages_loaded = 0;

void sigint_handler(int signo) {
    if(using_semaphore == 1) {
        struct sembuf buffer;
        buffer.sem_num = 0;
        buffer.sem_op = -1;
        buffer.sem_flg = 0;
        if(semop(semaphoreID, &buffer, 1) == -1) {
            printf("[LOADER] Error while releasing semaphore\n");
            exit(-1);
        }
        printf("[LOADER] Semaphore released, loader process finished\n");
    }
    printf("[LOADER] Total packages loaded: %d\n", total_packages_loaded);
    exit(0);
}

int main(int argc, char *argv[]) {
    struct sigaction sigact;
    sigact.sa_handler = sigint_handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, NULL);

    int N, C = -1;
    if(argc == 2) {
        if(sscanf(argv[1], "%d", &N) != 1){
            printf("[LOADER] Wrong first argument (N), expected int\n");
            exit(-1);
        }
    }
    else if(argc == 3) {
        if(sscanf(argv[1], "%d", &N) != 1){
            printf("[LOADER] Wrong first argument (N), expected int\n");
            exit(-1);
        }
        if(sscanf(argv[2], "%d", &C) != 1){
            printf("[LOADER] Wrong second argument (C), expected int\n");
            exit(-1);
        }
    }
    else {
        printf("[LOADER] Wrong number of arguments\n");
        exit(-1);
    }

    semaphoreID = semget(IDENTIFIER, 0, 0);
    if(semaphoreID < 0) {
        printf("[LOADER] Cannot obtain a semaphore\n");
        exit(-1);
    }

    memoryID = shmget(IDENTIFIER, 0, 0);
    if(memoryID < 0) {
        printf("[LOADER] Cannot obtain shared memory\n");
        exit(-1);
    }

    queue = shmat(memoryID, NULL, 0);
    if(queue == (void*) -1) {
        printf("[LOADER] Cannot attach shared memory to address space\n");
        exit(-1);
    }

    struct sembuf buffer;
    buffer.sem_num = 0;
    buffer.sem_op = -1;
    buffer.sem_flg = 0;

    struct Package my_package;
    struct tms spec;
    my_package.loader = getpid();
    my_package.weight = N;
    my_package.onLineTime = -1;

    int loop_controller = (C == -1) ? 0 : 1;
    int state = 0;
    while(C) {
        using_semaphore = 1;
        if(semop(semaphoreID, &buffer, 1) == -1) {
            printf("[LOADER] Error while taking semaphore\n");
            exit(-1);
        }
        if(my_package.onLineTime == -1) {
            my_package.onLineTime = times(&spec);
        }
        if(queue->currentSize < queue->lineSize) {
            if(queue->currentWeight + N > queue->maxWeight) {
                if(state != 1) {
                    printf("[LOADER] Cannot add package becouse of weight limit\n");
                    state = 1;
                }
            }
            else {
                enqueue(queue, my_package);
                printf("[LOADER] Loaded package with weight: %d\n", my_package.weight);
                C -= loop_controller;
                total_packages_loaded++;
                state = 0;
                my_package.onLineTime = -1;
            }
        }
        else {
            if(state != 2) {
                printf("[LOADER] Line is full, waiting for trucker to load packages\n");
                state = 2;
            }
        }

        buffer.sem_op = 1;
        if(semop(semaphoreID, &buffer, 1) == -1) {
            printf("[LOADER] Error while releasing semaphore\n");
            exit(-1);
        }
        using_semaphore = 0;
        buffer.sem_op = -1;
        usleep(10);
    }

    printf("[LOADER] Total packages loaded: %d\n", total_packages_loaded);

    return 0;
}