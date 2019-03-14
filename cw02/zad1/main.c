#define _POSIX_C_SOURCE 199309L

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>


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
    int file = open(filename, O_RDWR);
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


void copyLib(char* source, char* destination, unsigned int bytes, int records){
    FILE* sourceFile = fopen(source, "rb");
    FILE* destinationFile = fopen(destination, "wb");
    if(sourceFile == NULL){
        printf("Cannot find source file to copy\n");
        return;
    }
    if(destinationFile == NULL){
        printf("Cannot find destination file to copy\n");
        return;
    }
    fseek(sourceFile, 0, SEEK_SET);
    fseek(destinationFile, 0, SEEK_SET);
    int currentRecord = 0;
    unsigned char* buffer = (unsigned char*)malloc(bytes * sizeof(char));
    while(fread(buffer, sizeof(char), bytes, sourceFile) > 0 && currentRecord < records){
        fwrite(buffer, sizeof(char), bytes, destinationFile);
        currentRecord++;
    }
    fclose(sourceFile);
    fclose(destinationFile);
    free(buffer);
}


void copySys(char* source, char* destination, unsigned int bytes, unsigned int records){
    int sourceFile = open(source, O_RDONLY);
    int destinationFile = open(destination, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    if(sourceFile < 0){
        printf("Cannot find source file to copy\n");
        return;
    }
    if(destinationFile < 0){
        printf("Cannot find destination file to copy\n");
        return;
    }
    unsigned char* buffer = (unsigned char*)malloc(bytes * sizeof(char));
    int readBytes, currentRecord = 0;
    while((readBytes = read(sourceFile, buffer, bytes)) > 0 && currentRecord < records){
        write(destinationFile, buffer, readBytes);
    }
    close(sourceFile);
    close(destinationFile);
    free(buffer);
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
    /*unsigned int count = 100, bytes = 8;
    generate("randoms", count, bytes);
    printFile("randoms", bytes);
    sortSys("randoms", bytes);
    printFile("randoms", bytes);
    copySys("randoms", "randomsCopySys", 31, 500000);*/
    struct tms usTime;
    times(&usTime);
    char resultsFilename[128];
    char label[128];
    int savingResults = 0;
    for(int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "test") == 0) {
            if (i + 2 >= argc) {
                printf("%s%s%s",
                       "Incorrect argument in main, expected two strings (results filename and tests label) after: ",
                       argv[i], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%s", resultsFilename) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected string (results filename) after: ", argv[i - 1],
                       "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%s", label) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected string (tests label) as a second parameter of: ",
                       argv[i - 2], "\n");
                return -1;
            }
            savingResults = 1;
        } else if (strcmp(argv[i], "generate") == 0) {
            if (i + 3 >= argc) {
                printf("%s%s%s",
                       "Incorrect argument in main, expected string (file to save results) and two integers (records amount and bytes per record) after: ",
                       argv[i], "\n");
                return -1;
            }
            char filename[128];
            int recordsAmount, bytes;
            if (sscanf(argv[++i], "%s", filename) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected string (filename) after: ", argv[i - 1], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%d", &recordsAmount) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected integer (records amount) after: ", argv[i - 2], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%d", &bytes) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected integer (bytes per record) as a third parameter of: ", argv[i - 3], "\n");
                return -1;
            }
            if (recordsAmount <= 0 || bytes <= 0) {
                printf("Incorrect arguments, both records amount and bytes per record cannot be lower or equal to 0\n");
                return -1;
            }
            generate(filename, recordsAmount, bytes);
        }
        else if (strcmp(argv[i], "copy_sys") == 0) {
            if (i + 4 >= argc) {
                printf("%s%s%s",
                       "Incorrect argument in main, expected two strings (source and destination file names) and two integers (bytes in buffer and records to copy) after: ", argv[i], "\n");
                return -1;
            }
            char sourceFilename[128], destinationFilename[128];
            int bytes, records;
            if (sscanf(argv[++i], "%s", sourceFilename) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected string (source filename) after: ", argv[i - 1], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%s", destinationFilename) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected string (destination filename) after: ", argv[i - 2], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%d", &bytes) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected integer (bytes in buffer) as a third parameter of: ", argv[i - 3], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%d", &records) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected integer (records to copy) as a fourth parameter of: ", argv[i - 4], "\n");
                return -1;
            }
            if (bytes <= 0 || records <= 0) {
                printf("Incorrect argument, bytes in buffer and records to copy cannot be lower or equal to 0\n");
                return -1;
            }
            copySys(sourceFilename, destinationFilename, bytes, records);
        }
        else if (strcmp(argv[i], "copy_lib") == 0) {
            if (i + 4 >= argc) {
                printf("%s%s%s", "Incorrect argument in main, expected two strings (source and destination file names) and two integers (bytes in buffer and records to copy) after: ", argv[i], "\n");
                return -1;
            }
            char sourceFilename[128], destinationFilename[128];
            int bytes, records;
            if (sscanf(argv[++i], "%s", sourceFilename) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected string (source filename) after: ", argv[i - 1], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%s", destinationFilename) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected string (destination filename) after: ", argv[i - 2], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%d", &bytes) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected integer (bytes in buffer) as a third parameter of: ", argv[i - 3], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%d", &records) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected integer (records to copy) as a fourth parameter of: ", argv[i - 4], "\n");
                return -1;
            }
            if (bytes <= 0 || records <= 0) {
                printf("Incorrect argument, bytes in buffer and records to copy cannot be lower or equal to 0\n");
                return -1;
            }
            copyLib(sourceFilename, destinationFilename, bytes, records);
        }
        else if (strcmp(argv[i], "sort_sys") == 0) {
            if (i + 2 >= argc) {
                printf("%s%s%s", "Incorrect argument in main, string (name of file to sort) and integer (bytes per record) after: ", argv[i], "\n");
                return -1;
            }
            char filename[128];
            int bytes;
            if (sscanf(argv[++i], "%s", filename) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected string (name of file to sort) after: ", argv[i - 1], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%d", &bytes) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected integer (bytes per record) after: ", argv[i - 2], "\n");
                return -1;
            }
            if (bytes <= 0) {
                printf("Incorrect argument, bytes per record cannot be lower or equal to 0\n");
                return -1;
            }
            sortSys(filename, bytes);
        }
        else if (strcmp(argv[i], "sort_lib") == 0) {
            if (i + 2 >= argc) {
                printf("%s%s%s", "Incorrect argument in main, string (name of file to sort) and integer (bytes per record) after: ", argv[i], "\n");
                return -1;
            }
            char filename[128];
            int bytes;
            if (sscanf(argv[++i], "%s", filename) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected string (name of file to sort) after: ", argv[i - 1], "\n");
                return -1;
            }
            if (sscanf(argv[++i], "%d", &bytes) == EOF) {
                printf("%s%s%s", "Incorrect argument in main, expected integer (bytes per record) after: ", argv[i - 2], "\n");
                return -1;
            }
            if (bytes <= 0) {
                printf("Incorrect argument, bytes per record cannot be lower or equal to 0\n");
                return -1;
            }
            sortLib(filename, bytes);
        }
    }
    if(savingResults == 1) {
        struct tms usTimeEnd;
        times(&usTimeEnd);
        usTime.tms_utime = usTimeEnd.tms_utime - usTime.tms_utime;
        usTime.tms_stime = usTimeEnd.tms_stime - usTime.tms_stime;
        usTime.tms_cutime = usTimeEnd.tms_cutime - usTime.tms_cutime;
        usTime.tms_cstime = usTimeEnd.tms_cstime - usTime.tms_cstime;
        FILE* resultsFile = fopen(resultsFilename, "a");
        fprintf(resultsFile, "%s:\n", label);
        int clockTicks = sysconf(_SC_CLK_TCK);
        fprintf(resultsFile, "%s%f%s", "System time: ", usTime.tms_cstime/(float)clockTicks + usTime.tms_stime/(float)clockTicks, "\n");
        fprintf(resultsFile, "%s%f%s", "User time: ", usTime.tms_cutime/(float)clockTicks + usTime.tms_utime/(float)clockTicks, "\n\n");
    }

    return 0;
}
  

