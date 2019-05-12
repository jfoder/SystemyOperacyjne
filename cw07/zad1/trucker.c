#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include "header.h"


int memoryID;
int semaphoreID;
struct PackageQueue* queue;
int total_packages_loaded = 0;

void sigint_handler(int signo) {
    if(memoryID >= 0) {
        shmctl(memoryID, IPC_RMID, NULL);
    }
    if(semaphoreID >= 0) {
        shmdt(queue);
        semctl(semaphoreID, 0, IPC_RMID);
    }
    printf("[TRUCKER] Total packages loaded: %d\n", total_packages_loaded);
    exit(0);
}


int main(int argc, char *argv[]) {
    struct sigaction sigact;
    sigact.sa_handler = sigint_handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, NULL);

    int X, K, M, currentTruckLoad = 0;
    if(argc != 4){
        printf("[TRUCKER] Wrong arguments, expected: X, K, M\n");
        exit(-1);
    }
    if(sscanf(argv[1], "%d", &X) != 1){
        printf("[TRUCKER] Wrong first argument (X), expected int\n");
        exit(-1);
    }
    if(sscanf(argv[2], "%d", &K) != 1){
        printf("[TRUCKER] Wrong second argument (K), expected int\n");
        exit(-1);
    }
    if(sscanf(argv[3], "%d", &M) != 1){
        printf("[TRUCKER] Wrong third argument (M), expected int\n");
        exit(-1);
    }

    memoryID = shmget(IDENTIFIER, sizeof(struct PackageQueue), IPC_EXCL | IPC_CREAT | 0666);
    if(memoryID < 0) {
        printf("[TRUCKER] Cannot create shared memory\n");
        exit(-1);
    }

    semaphoreID = semget(IDENTIFIER, 1, IPC_EXCL | IPC_CREAT | 0666);
    if(semctl(semaphoreID, 0, SETVAL, 1) == -1) {
        printf("[TRUCKER] Error while setting initial semaphore value\n");
        exit(-1);
    }

    queue = shmat(memoryID, NULL, 0);
    queue->front = 0;
    queue->rear = 0;
    queue->currentSize = 0;
    queue->lineSize = K;
    queue->currentWeight = 0;
    queue->maxWeight = M;

    struct sembuf buffer;
    buffer.sem_num = 0;
    buffer.sem_op = -1;
    buffer.sem_flg = 0;

    struct tms spec;
    int state = 0;
    while(1) {
        if(semop(semaphoreID, &buffer, 1) == -1) {
            printf("[TRUCKER] Error while taking semaphore\n");
            exit(-1);
        }
        if(currentTruckLoad >= X) {
            printf("[TRUCKER] Truck is full, emptying...\n");
            currentTruckLoad = 0;
            printf("[TRUCKER] Truck is now empty\n");
            state = 0;
        }
        if(queue->currentSize > 0) {
            if(currentTruckLoad + queue->packages[(queue->front) % queue->lineSize].weight > X) {
                printf("[TRUCKER] Cannot load next package becouse of truck weight limit. Emptying the truck...\n");
                currentTruckLoad = 0;
                printf("[TRUCKER] Truck is now empty\n");
            }
            else {
                struct Package p = dequeue(queue);
                currentTruckLoad += p.weight;
                total_packages_loaded++;
                printf("[TRUCKER] [%d/%d] Loaded package from worker with ID: %d, which weights: %d and was loaded at: %ldms\n", currentTruckLoad, X, p.loader, p.weight, times(&spec) - p.onLineTime);
            }
            state = 0;
        }
        else {
            if(state != 1) {
                printf("[TRUCKER] Line is empty, waiting for packages\n");
                state = 1;
            }
        }

        buffer.sem_op = 1;
        if(semop(semaphoreID, &buffer, 1) == -1) {
            printf("[TRUCKER] Error while releasing semaphore\n");
            exit(-1);
        }
        buffer.sem_op = -1;
    }
}