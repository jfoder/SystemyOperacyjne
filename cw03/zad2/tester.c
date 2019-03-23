#define _POSIX_C_SOURCE 199309L

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/times.h>



int main(int argc, char* argv[]){
    srand(time(NULL));
    FILE* to_edit;
    char filename[256];
    int pmin, pmax, bytes;
    int limit = 10;
    if(argc != 5){
        printf("Incorrect number of parameters. Expected name of the file to edit (string), pmin (int), pmax (int), bytes (int)\n");
        return -1;
    }
    if(sscanf(argv[1], "%s", filename) == EOF){
        printf("Incorrect first argument. Expected name of file with to edit\n");
        return -1;
    }
    if(sscanf(argv[2], "%d", &pmin) == EOF){
        printf("Incorrect second argument. Expected positive integer (pmin)\n");
        return -1;
    }
    if(sscanf(argv[3], "%d", &pmax) == EOF){
        printf("Incorrect third argument. Expected positive integer (pmax)\n");
        return -1;
    }
    if(sscanf(argv[4], "%d", &bytes) == EOF){
        printf("Incorrect fourth argument. Expected positive integer (bytes)\n");
        return -1;
    }


    int i = 0;
    int range = pmax - pmin + 1;
    while(i < limit){
        to_edit = fopen(filename, "a");
        if(to_edit == NULL){
            printf("Cannot open file: %s\n", filename);
            return -1;
        }
        int to_wait = rand()%range + pmin;
        printf("%d\n", to_wait);
        sleep(to_wait);
        int pid = getpid();
        time_t t = time(NULL);
        char date[40];
        strftime(date, 40, "%Y-%m-%d %H:%M:%S", localtime(&t));
        char* text = malloc((1 + bytes) * sizeof(char));
        for(int j = 0; j < bytes; j++) {
            text[j] = (char)rand()%26 + 65;
        }
        text[bytes] = '\0';
        printf("%s\n", text);
        char line[1024];
        sprintf(line, "%d %d %s %s", pid, to_wait, date, text);
        strcat(line, "\n");
        int index = 0;
        for(int j = 0; j < 1024; j++){
            if(line[j] == '\n'){
                index = j;
                break;
            }
        }
        free(text);
        printf("%d\n", index);
        fwrite(line, sizeof(char), index + 1, to_edit);
        i++;
        fclose(to_edit);
    }

    return 0;
}
  

