#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <dlfcn.h>
#ifndef DLL
#include "findLib.h"
#endif

#ifdef DLL
struct blocksArray{
  char** array;
  int size;
};
#endif


int main(int argc, char* argv[]){
  #ifdef DLL
  void* handle = dlopen("./findLib.so", RTLD_LAZY);
  if(handle == NULL){
    printf("Cannot load dynamic library\n");
    return -1;
  }
  struct blocksArray* (*createArray)(int) = dlsym(handle, "createArray");
  void (*setSearchingParameters)(char*, char*) = dlsym(handle, "setSearchingParameters");
  void (*search)(char*) = dlsym(handle, "search");
  int (*allocateBlock)(struct blocksArray*, char*) = dlsym(handle, "allocateBlock");
  void (*deleteBlock)(struct blocksArray*, int) = dlsym(handle, "deleteBlock");
  #endif
  
  struct blocksArray* array = NULL;
  int parametersPassed = 0;
  char directory[128];
  char filename[128];
  struct timespec realTime;
  realTime.tv_sec = 0;
  realTime.tv_nsec = 0;
  struct tms usTime;
  usTime.tms_utime = 0;
  usTime.tms_stime = 0;
  usTime.tms_cutime = 0;
  usTime.tms_cstime = 0;
  char resultsFilename[128];
  char label[128];
  int savingResults = 0;
  for(int i = 1; i < argc; i++){
    if(strcmp(argv[i], "test") == 0){
      if(i + 2 >= argc){
	printf("%s%s%s", "Incorrect argument in main, expected two strings (results filename and tests label) after: ", argv[i], "\n");
	return -1;
      }
      if(sscanf(argv[++i], "%s", resultsFilename) == EOF){
	printf("%s%s%s", "Incorrect argument in main, expected string (results filename) after: ", argv[i - 1], "\n");
	return -1;
      }
      if(sscanf(argv[++i], "%s", label) == EOF){
	printf("%s%s%s", "Incorrect argument in main, expected string (tests label) as a second parameter of: ", argv[i - 2], "\n");
	return -1;
      }
      savingResults = 1;
    }
    else if(strcmp(argv[i], "create_array") == 0){
      printf("Creating array\n");
      i++;
      if(i >= argc) {
	printf("%s%s%s", "Incorrect argument in main, expected integer after: ", argv[i - 1], "\n");
	return -1;
      }
      int arraySize;
      if(sscanf(argv[i], "%d", &arraySize) == EOF) {
	printf("%s%s%s", "Incorrect argument in main, expected integer after: ", argv[i - 1], "\n");
	return -1;
      }
      if(arraySize <= 0) {
	printf("Array size cannot be equal or lower than zero\n");
	return -1;
      }
      if(array != NULL){
	printf("Array have been already created\n");
	return -1;
      }
      array = createArray(arraySize);
    }
    else if(strcmp(argv[i], "set_find_parameters") == 0){
      printf("Setting \"find\" parameters\n");
      if(i + 2 >= argc){
        printf("%s%s%s", "Incorrect argument in main, expected two strings after: ", argv[i], "\n");
	return -1;
      }
      if(sscanf(argv[++i], "%s", directory) == EOF || sscanf(argv[++i], "%s", filename) == EOF){
	printf("Incorrect arguments after \"set_find_parameters\", expected two strings: directory and filename\n");
	return -1;
      }
      setSearchingParameters(directory, filename);
      parametersPassed = 1;
    }
    else if(strcmp(argv[i], "search_directory") == 0){
      struct timespec realTimeBegin;
      struct tms usTimeBegin;
      clock_gettime(CLOCK_REALTIME, &realTimeBegin);
      times(&usTimeBegin);
      printf("Searching directory\n");
      if(parametersPassed == 0){
	printf("You have to set find parameters before searching directory\n");
	return -1;
      }
      if(++i  >= argc){
	printf("%s%s%s", "Incorrect argument in main, expected string (temporary file name) after: ", argv[i - 1], "\n");
	return -1;
      }
      char tmpFilename[128];
      if(sscanf(argv[i], "%s", tmpFilename) == EOF){
	printf("Incorrect argument after \"search_directory\", expected string\n");
	return -1;
      }
      search(tmpFilename);
      struct timespec realTimeEnd;
      struct tms usTimeEnd;
      clock_gettime(CLOCK_REALTIME, &realTimeEnd);
      times(&usTimeEnd);
      realTime.tv_sec += realTimeEnd.tv_sec - realTimeBegin.tv_sec;
      realTime.tv_nsec += realTimeEnd.tv_nsec - realTimeBegin.tv_nsec;
      usTime.tms_utime += usTimeEnd.tms_utime - usTimeBegin.tms_utime;
      usTime.tms_stime += usTimeEnd.tms_stime - usTimeBegin.tms_stime;
      usTime.tms_cutime += usTimeEnd.tms_cutime - usTimeBegin.tms_cutime;
      usTime.tms_cstime += usTimeEnd.tms_cstime - usTimeBegin.tms_cstime;
    }
    else if(strcmp(argv[i], "allocate_memory") == 0){
      struct timespec realTimeBegin;
      struct tms usTimeBegin;
      clock_gettime(CLOCK_REALTIME, &realTimeBegin);
      times(&usTimeBegin);
      printf("Allocating block of memory\n");
      if(array == NULL){
	printf("Array has to be created before memory allocation\n");
	return -1;
      }
      if(++i > argc){
	printf("%s%s%s", "Incorrect argument in main, expected string (temporary file name) after command: ", argv[i - 1], "\n");
	return -1;
      }
      char tmpFilename[128];
      if(sscanf(argv[i], "%s", tmpFilename) == EOF){
	printf("Incorrect argument after \"allocate_memory\", expected string (temporary file name)\n");
	return -1;
      }
      int index = allocateBlock(array, tmpFilename);
      if(index != -1) printf("%s%d%s", "Block of memory allocated at index: ", index, "\n");
      else return -1;
      struct timespec realTimeEnd;
      struct tms usTimeEnd;
      clock_gettime(CLOCK_REALTIME, &realTimeEnd);
      times(&usTimeEnd);
      realTime.tv_sec += realTimeEnd.tv_sec - realTimeBegin.tv_sec;
      realTime.tv_nsec += realTimeEnd.tv_nsec - realTimeBegin.tv_nsec;
      usTime.tms_utime += usTimeEnd.tms_utime - usTimeBegin.tms_utime;
      usTime.tms_stime += usTimeEnd.tms_stime - usTimeBegin.tms_stime;
      usTime.tms_cutime += usTimeEnd.tms_cutime - usTimeBegin.tms_cutime;
      usTime.tms_cstime += usTimeEnd.tms_cstime - usTimeBegin.tms_cstime;
    }
    else if(strcmp(argv[i], "delete_block") == 0){
      struct timespec realTimeBegin;
      struct tms usTimeBegin;
      clock_gettime(CLOCK_REALTIME, &realTimeBegin);
      times(&usTimeBegin);
      printf("Deleting block from array\n");
      if(array == NULL){
	printf("Array has to be created before deleting blocks\n");
	return -1;
      }
      if(++i > argc){
	printf("%s%s%s", "Incorrect argument in main, expected int after command: ", argv[i - 1], "\n");
	return -1;
      }
      int blockIndex;
      sscanf(argv[i], "%d", &blockIndex);
      if(blockIndex < 0 || blockIndex > array->size - 1){
	printf("Deleting block index out of array bounds\n");
	return -1;
      }
      deleteBlock(array, blockIndex);
      struct timespec realTimeEnd;
      struct tms usTimeEnd;
      clock_gettime(CLOCK_REALTIME, &realTimeEnd);
      times(&usTimeEnd);
      realTime.tv_sec += realTimeEnd.tv_sec - realTimeBegin.tv_sec;
      realTime.tv_nsec += realTimeEnd.tv_nsec - realTimeBegin.tv_nsec;
      usTime.tms_utime += usTimeEnd.tms_utime - usTimeBegin.tms_utime;
      usTime.tms_stime += usTimeEnd.tms_stime - usTimeBegin.tms_stime;
      usTime.tms_cutime += usTimeEnd.tms_cutime - usTimeBegin.tms_cutime;
      usTime.tms_cstime += usTimeEnd.tms_cstime - usTimeBegin.tms_cstime;
    }
  }
  if(savingResults == 1){
    FILE* resultsFile = fopen(resultsFilename, "a");
    fprintf(resultsFile, "%s:\n", label);
    fprintf(resultsFile, "%s%f%s", "Real time: ", (float)(realTime.tv_sec) + (realTime.tv_nsec)/1000000000.0, "\n");
    int clockTicks = sysconf(_SC_CLK_TCK);
    fprintf(resultsFile, "%s%f%s", "System time: ", usTime.tms_cstime/(float)clockTicks + usTime.tms_stime/(float)clockTicks, "\n");
    fprintf(resultsFile, "%s%f%s", "User time: ", usTime.tms_cutime/(float)clockTicks + usTime.tms_utime/(float)clockTicks, "\n\n");
  }
  if(array != NULL){
    for(int i = 0; i < array->size; i++){
      free(array->array[i]);
    }
    free(array->array);
  }
  #ifdef DLL
  dlclose(handle);
  #endif
  return 0;
}
  

