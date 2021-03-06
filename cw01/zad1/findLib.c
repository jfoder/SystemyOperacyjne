#include "findLib.h"

char *directory;
char *filename;

struct blocksArray *createArray(int size) {
    if (size <= 1) {
        printf("Array size cannot be lower than 1\n");
        return NULL;
    }
    struct blocksArray *result = malloc(sizeof(struct blocksArray));
    if(result == NULL){
        printf("Memory allocation error\n");
        return NULL;
    }
    result->size = size;
    result->array = (char **) calloc(size, sizeof(char *));
    if(result->array == NULL){
        printf("Memory allocation error\n");
        return NULL;
    }
    return result;
}

void setSearchingParameters(char *directoryToSearch, char *filenameToFind) {
    if (!(directoryToSearch && filenameToFind)) {
        printf("Both directory and file names cannot be empty\n");
    }
    directory = directoryToSearch;
    filename = filenameToFind;
}

void search(char *tmpFile) {
    int size = (17 + strlen(directory) + strlen(filename) + strlen(tmpFile));
    char *command = (char *) malloc(sizeof(char) * size);
    sprintf(command, "%s%s%s%s%s%s", "find ", directory, " -name ", filename, " > ", tmpFile);
    command[size - 1] = '\0';
    printf("Done\n");
    system(command);
}

int allocateBlock(struct blocksArray* bArray, char* tmpFilename) {
    FILE* tmpFile = fopen(tmpFilename, "r");
    if(bArray == NULL){
        printf("Array of blocks cannot be NULL\n");
        return -1;
    }
    if(tmpFile == NULL){
        printf("Cannot find selected temporary file\n");
        return -1;
    }
    fseek(tmpFile, 0, SEEK_END);
    int tmpSize = ftell(tmpFile);
    fseek(tmpFile, 0, SEEK_SET);
    int i = 0;
    while(i < bArray->size){
        if(bArray->array[i] == NULL) break;
        i++;
    }
    if(i >= bArray->size){
        printf("No empty block in array, need to free some memory block\n");
        return -1;
    }
    bArray->array[i] = calloc(tmpSize, sizeof(char));
    if(bArray->array[i] == NULL){
        printf("Memory allocation error during inserting temporary file content into memory block\n");
        return -1;
    }
    fread(bArray->array[i], tmpSize, 1, tmpFile);
    fclose(tmpFile);
    return i;
}

void deleteBlock(struct blocksArray* bArray, int blockIndex){
    if(bArray == NULL){
        printf("Array of blocks cannot be NULL\n");
        return;
    }
    if(blockIndex < 0 || blockIndex >= bArray->size){
        printf("Index out of array bounds\n");
        return;
    }
    if(bArray->array[blockIndex] == NULL){
        printf("Selected block is already empty\n");
        return;
    }
    free(bArray->array[blockIndex]);
    bArray->array[blockIndex] = NULL;
}
  

