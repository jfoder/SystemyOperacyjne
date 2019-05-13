#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "header.h"


int memoryID;
sem_t* semaphoreID;
struct PackageQueue* queue;
int total_packages_loaded = 0;

void sigint_handler(int signo) {
    if(memoryID >= 0) {
        munmap(queue, sizeof(struct PackageQueue));
        shm_unlink(MEMORY_NAME);
    }
    if(semaphoreID >= 0) {
        sem_close(semaphoreID);
        sem_unlink(MEMORY_NAME);
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

    memoryID = shm_open(MEMORY_NAME, O_RDWR | O_CREAT, 0666);
    if(memoryID < 0) {
        printf("[TRUCKER] Cannot create shared memory\n");
        exit(-1);
    }
    if(ftruncate(memoryID, sizeof(struct PackageQueue)) < 0) {
        printf("[TRUCKER] Error while specifying shared memory size\n");
        exit(-1);
    }

    semaphoreID = sem_open(MEMORY_NAME, O_RDWR | O_CREAT, 0666, 1);
    if(semaphoreID == SEM_FAILED) {
        printf("[TRUCKER] Error while creating semaphore\n");
        exit(-1);
    }

    queue = mmap(NULL, sizeof(struct PackageQueue), PROT_READ | PROT_WRITE, MAP_SHARED, memoryID, 0);
    if(queue == (void*) -1) {
        printf("[TRUCKER] Error while joining shared memory to address space\n");
        exit(-1);
    }
    queue->front = 0;
    queue->rear = 0;
    queue->currentSize = 0;
    queue->lineSize = K;
    queue->currentWeight = 0;
    queue->maxWeight = M;

    struct tms spec;
    int state = 0;
    while(1) {
        if(sem_wait(semaphoreID) < 0) {
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

        if(sem_post(semaphoreID) < 0) {
            printf("[TRUCKER] Error while releasing semaphore\n");
            exit(-1);
        }
    }
}