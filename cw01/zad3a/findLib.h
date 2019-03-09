#ifndef EXAMPLE_LIBRARY_H
#define EXAMPLE_LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct blocksArray{
  char** array;
  int size;
};

struct blocksArray* createArray(int size);
void setSearchingParameters(char* directoryToSearch, char* filenameToFind);
void search(char* tmpFile);
int allocateBlock(struct blocksArray* array, char* tmpFilename);
void deleteBlock(struct blocksArray* array, int blockIndex);

#endif
