#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <math.h>
#include <time.h>


typedef struct thread_arguments {
    int a;
    int b;
} thread_arguments;


int N, threads;
int** input_image;
int** output_image;
double** filter;
int w, h;
int mode;
int filter_size;


void* filtering(void* args) {
    struct tms spec;
    long int end;
    long int begin = times(&spec);
    struct thread_arguments* arguments = (struct thread_arguments*) args;
    if(mode == 0) {
        for(int x = arguments->a; x <= arguments->b; x++) {
            for(int y = 0; y < h; y++) {
                double px = 0;
                for (int j = 0; j < filter_size; j++) {
                    int b = (int)round(fmax(0, y - ceil((double)filter_size / 2) + j));
                    b = b < h ? b : h - 1;
                    for (int i = 0; i < filter_size; i++) {
                        int a = (int)round(fmax(0, x - ceil((double)filter_size / 2) + i));
                        a = a < w ? a : w - 1;
                        double v = input_image[b][a] * filter[j][i];
                        px += v;
                    }
                }
                px = px < 0 ? 0 : px;
                output_image[y][x] = (int)round(px);
            }
        }
    }
    else if(mode == 1) {
        for(int x = arguments->a; x < w; x += arguments->b) {
            for(int y = 0; y < h; y++) {
                double px = 0;
                for (int j = 0; j < filter_size; j++) {
                    int b = (int)round(fmax(0, y - ceil((double)filter_size / 2) + j));
                    b = b < h ? b : h - 1;
                    for (int i = 0; i < filter_size; i++) {
                        int a = (int)round(fmax(0, x - ceil((double)filter_size / 2) + i));
                        a = a < w ? a : w - 1;
                        double v = input_image[b][a] * filter[j][i];
                        px += v;
                    }
                }
                px = px < 0 ? 0 : px;
                output_image[y][x] = (int)round(px);
            }
        }
    }
    end = times(&spec);
    long int* result = malloc(sizeof(long int));
    *result = (end - begin);
    //usleep(10);
    pthread_exit(result);
    return NULL;
}


int main(int argc, char *argv[]) {
    if(argc != 6) {
        printf("Wrong number of arguments. Expected 5 arguments: number of threads, mode (0 - block, 1 - interval), input filename, filter filename, output filename\n");
        exit(-1);
    }
    if(sscanf(argv[1], "%d", &threads) != 1) {
        printf("Incorrect first argument. Expected number of threads\n");
        exit(-1);
    }
    if(sscanf(argv[2], "%d", &mode) != 1 || mode < 0 || mode > 1) {
        printf("Incorrect second argument. Expected mode (0 - block, 1 - interval)\n");
        exit(-1);
    }

    FILE* input_file = fopen(argv[3], "r");
    if(input_file == NULL) {
        printf("Cannot open input file: %s\n", argv[3]);
        exit(-1);
    }
    FILE* filter_file = fopen(argv[4], "r");
    if(filter_file == NULL) {
        printf("Cannot open filter file: %s\n", argv[4]);
        exit(-1);
    }
    FILE* output_file = fopen(argv[5], "w");
    if(output_file == NULL) {
        printf("Cannot open output file: %s\n", argv[5]);
        exit(-1);
    }


    char buff[512];
    int buff_size = 512;
    fgets(buff, buff_size, input_file);
    fgets(buff, buff_size, input_file);
    char *dimens;
    dimens = strdup(buff);
    w = (int)strtol(strsep(&dimens, " \t"), NULL, 10);
    h = (int)strtol(strsep(&dimens, " \t"), NULL, 10);


    input_image = calloc((size_t) h, sizeof(int *));
    for(int i = 0; i < h; i++) {
        input_image[i] = calloc((size_t) w, sizeof(int));
    }

    output_image = calloc((size_t) h, sizeof(int *));
    for (int i = 0; i < h; i++) {
        output_image[i] = calloc((size_t) w, sizeof(int));
    }

    fgets(buff, buff_size, input_file);

    int iter_column = 0;
    int iter_row = 0;
    while(fgets(buff, buff_size, input_file) != NULL) {
        for(char *word = strtok(buff, " \n\t\r"); word != NULL; word = strtok(NULL, " \t\n\r")) {
            input_image[iter_row][iter_column] = (int)strtol(word, NULL, 10);
            iter_column++;
            if (iter_column == w) {
                iter_row++;
                iter_column = 0;
            }
        }
    }


    fgets(buff, buff_size, filter_file);
    filter_size = (int)strtol(buff, NULL, 10);


    filter = calloc((size_t)filter_size, sizeof(double *));

    for(int i = 0; i < filter_size; i++) {
        filter[i] = calloc((size_t) filter_size, sizeof(double));
        iter_column++;
        if (iter_column == filter_size) {
            iter_row++;
            iter_column = 0;
        }
    }

    iter_column = 0;
    iter_row = 0;
    while(fgets(buff, buff_size, filter_file) != NULL) {
        for(char *word = strtok(buff, " \n\t\r"); word != NULL; word = strtok(NULL, " \t\n\r")) {
            filter[iter_row][iter_column] = strtod(word, NULL);
            iter_column++;
            if (iter_column == filter_size) {
                iter_row++;
                iter_column = 0;
            }
        }
    }

    pthread_t *threads_array = calloc((size_t) threads, sizeof(pthread_t));
    struct thread_arguments** threads_arguments = malloc(threads * sizeof(struct thread_info*));

    for(int i = 0; i < threads; i++) {
        threads_arguments[i] = malloc(sizeof(struct thread_arguments));
        if(mode == 0) {
            threads_arguments[i]->a = (i * w / threads);
            threads_arguments[i]->b = ((i + 1) * w / threads) - 1;
        }
        else if(mode == 1) {
            threads_arguments[i]->a = i;
            threads_arguments[i]->b = threads;
        }
    }


    struct tms spec;
    long int end;
    long int begin = times(&spec);

    for(int i = 0; i < threads; i++) {
        pthread_create(&threads_array[i], NULL, filtering, (void*) threads_arguments[i]);
    }

    long int* result;
    for(int i = 0; i < threads; i++) {
        pthread_join(threads_array[i], (void**)&result);
        printf("[%d] %ldms\n", i, *result);
        free(result);
        free(threads_arguments[i]);
    }

    free(threads_arguments);


    end = times(&spec);
    printf("Total time: %ldms\n", (end - begin));


    char buff2[1024];
    fprintf(output_file, "P2\n");
    fprintf(output_file, "%d %d\n", w, h);
    fprintf(output_file, "%d\n", 255);
    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            if (j < w - 1) {
                sprintf(buff2, "%d ", output_image[i][j]);
            }
            else {
                sprintf(buff2, "%d\n", output_image[i][j]);
            }
            fputs(buff2, output_file);
        }
    }

    free(threads_array);

    for(int i = 0; i < h; i++) {
        free(input_image[i]);
    }
    for (int i = 0; i < h; i++) {
        free(output_image[i]);
    }
    free(input_image);
    free(output_image);


    fclose(input_file);
    fclose(filter_file);
    fclose(output_file);
}