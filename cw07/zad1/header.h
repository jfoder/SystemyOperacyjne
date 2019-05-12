#include <sys/param.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stddef.h>
#include <string.h>

#define MAX_QUEUE_SIZE 1000
#define IDENTIFIER ftok(getenv("HOME"), 31)

struct Package {
    pid_t loader;
    int weight;
    long int onLineTime;
};

struct PackageQueue {
    struct Package packages[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int currentSize;
    int lineSize;
    int currentWeight;
    int maxWeight;
};

void enqueue(struct PackageQueue* queue, struct Package package) {
    if(queue->currentSize >= queue->lineSize) {
        printf("Queue overflow\n");
        exit(-1);
    }
    queue->packages[(queue->rear++) % queue->lineSize] = package;
    queue->currentSize++;
    queue->currentWeight += package.weight;
    if(queue->currentWeight > queue->maxWeight) {
        printf("Cannot add package to line becouse of max load limit\n");
        exit(-1);
    }
}

struct Package dequeue(struct PackageQueue* queue) {
    if(queue->currentSize <= 0) {
        printf("Cannot dequeue from empty queue\n");
        exit(-1);
    }
    struct Package result = queue->packages[(queue->front) % queue->lineSize];
    queue->front++;
    queue->currentWeight -= result.weight;
    queue->currentSize--;
    return result;
}