#include <stdio.h>
#include <string.h>
#include "findLib.h"


int main(int argc, char* argv[]){
  struct blocksArray* array;
  int parametersPassed = 0;
  char directory[128];
  char filename[128];
  for(int i = 1; i < argc; i++){
    if(strcmp(argv[i], "create_array") == 0){
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
	printf("Searching directory\n");
	if(parametersPassed == 0){
	  printf("You have to set find parameters before searching directory\n");
	  return -1;
	}
	if(++i  >= argc){
	  printf("%s%s%s", "Incorrect argument in main, expected string (temporary file name) after: ", argv[i], "\n");
	  return -1;
	}
	char tmpFilename[128];
	if(sscanf(argv[i], "%s", tmpFilename) == EOF){
	  printf("Incorrect argument after \"search_directory\", expected string\n");
	  return -1;
	}
	search(tmpFilename);
    }
  }
    
  return 0;
}
  

