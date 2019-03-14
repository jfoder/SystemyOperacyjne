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
    unsigned char* tmp = (unsigned char*)malloc(bytes * sizeof(char));
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
    if(file == NULL){
        printf("Cannot find file: %s\n", filename);
        return;
    }
    fseek(file, 0, SEEK_END);
    unsigned int count = ftell(file)/bytes;
    if(ftell(file) % bytes != 0){
        printf("Not enough bytes in file\n");
        return;
    }
    fseek(file, 0, SEEK_SET);
    unsigned char* tmp1 = (unsigned char*)malloc(bytes * sizeof(char));
    unsigned char* tmp2 = (unsigned char*)malloc(bytes * sizeof(char));
    for(int i = 0; i < count; i++){
        fseek(file, i * bytes, SEEK_SET);
        fread(tmp2, sizeof(char), bytes, file);
        unsigned char currentMin = tmp2[0];
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


void sortSys(char* filename, unsigned int bytes){
    unsigned int file = open(filename, O_RDWR);
    if(file < 0){
        printf("Cannot find file: %s\n", filename);
        return;
    }
    long fileSize = lseek(file, 0, SEEK_END);
    long count = fileSize/bytes;
    if(fileSize % bytes != 0){
        printf("Not enough bytes in file\n");
        return;
    }
    lseek(file, 0, SEEK_SET);
    unsigned char* tmp1 = (unsigned char*)malloc(bytes * sizeof(char));
    unsigned char* tmp2 = (unsigned char*)malloc(bytes * sizeof(char));
    for(int i = 0; i < count; i++){
        lseek(file, i * bytes, SEEK_SET);
        read(file, tmp2, bytes);
        unsigned char currentMin = tmp2[0];
        unsigned int minIndex = i;
        for(int j = i + 1; j < count; j++){
            read(file, tmp1, bytes);
            if(tmp1[0] < currentMin){
                currentMin = tmp1[0];
                minIndex = j;
            }
        }
        if(minIndex != i) {
            lseek(file, minIndex * bytes, SEEK_SET);
            read(file, tmp1, bytes);
            lseek(file, minIndex * bytes, SEEK_SET);
            write(file, tmp2, bytes);
            lseek(file, i * bytes, SEEK_SET);
            write(file, tmp1, bytes);
        }
    }
    free(tmp1);
    free(tmp2);
    close(file);
}


void generate(char* filename, unsigned int count, unsigned int bytes){
    FILE* randomBytes = fopen(filename, "w");
    unsigned char* toSave = (unsigned char*)malloc(bytes * sizeof(char));
    for(int i = 0; i < count; i++){
        for(int j = 0; j < bytes; j++){
            toSave[j] = (char)(rand()%256);
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
    sortSys("randoms", bytes);
    printFile("randoms", bytes);
    return 0;
}
  

