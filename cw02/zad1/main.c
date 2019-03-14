#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


void printFile(char* filename, unsigned int bytes){
    FILE* file = fopen(filename, "r+b");
    fseek(file, 0, SEEK_END);
    unsigned int count = ftell(file)/bytes;
    char* tmp = (char*)malloc(bytes * sizeof(char));
    fseek(file, 0, SEEK_SET);
    for(int i = 0; i < count; i++){
        fread(tmp, sizeof(char), bytes, file);
        for(int j = 0; j < bytes; j++){
            printf("%d ", (int)tmp[j]);
        }
        printf("\n");
    }
    fclose(file);
    printf("\n\n");
}

void sortLib(char* filename, unsigned int bytes){
    FILE* file = fopen(filename, "r+b");
    fseek(file, 0, SEEK_END);
    unsigned int count = ftell(file)/bytes;
    if(ftell(file) % bytes != 0){
        printf("Brak odpowiedniej liczby bajtow\n");
        //return -1;
    }
    fseek(file, 0, SEEK_SET);
    char* tmp1 = (char*)malloc(bytes * sizeof(char));
    char* tmp2 = (char*)malloc(bytes * sizeof(char));
    for(int i = 0; i < count; i++){
        fseek(file, i * bytes, SEEK_SET);
        fread(tmp2, sizeof(char), bytes, file);
        char currentMin = tmp2[0];
        unsigned int minIndex = i;
        for(int j = i + 1; j < count; j++){
            fread(tmp1, sizeof(char), bytes, file);
            if(tmp1[0] < currentMin){
                currentMin = tmp1[0];
                minIndex = j;
            }
        }
        if(minIndex != i) {
            fseek(file, minIndex * bytes, SEEK_SET);
            fread(tmp1, sizeof(char), bytes, file);
            fseek(file, minIndex * bytes, SEEK_SET);
            fwrite(tmp2, sizeof(char), bytes, file);
            fseek(file, i * bytes, SEEK_SET);
            fwrite(tmp1, sizeof(char), bytes, file);
        }
    }
    free(tmp1);
    free(tmp2);
    fclose(file);
}

void generate(char* filename, unsigned int count, unsigned int bytes){
    FILE* randomBytes = fopen(filename, "w");
    char* toSave = (char*)malloc(bytes * sizeof(char));
    for(int i = 0; i < count; i++){
        for(int j = 0; j < bytes; j++){
            toSave[j] = (char)(rand()%256 - 128);
        }
        fwrite (toSave , sizeof(char), bytes, randomBytes);
    }
    fclose(randomBytes);
    free(toSave);
}


int main(int argc, char* argv[]){
    srand(time(NULL));
    unsigned int count = 100, bytes = 8;
    generate("randoms", count, bytes);
    printFile("randoms", bytes);
    sortLib("randoms", bytes);
    printFile("randoms", bytes);
    char c = 127;
    printf("%d\n", c);
    return 0;
}
  

