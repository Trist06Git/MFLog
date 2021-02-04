
#include <stdio.h>
#include <stdint.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define TEST_SIZE 21

extern int errno;

//realloc vs mremap, as per manual pages who claim mremap its faster...
//and indeed it is faster.
int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());
    printf("test size: %i\n", TEST_SIZE);

    size_t page_size = getpagesize();
    int current_length = 1;

    uint8_t* chunk = mmap(
        NULL,
        page_size*current_length,
        PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_ANONYMOUS|MAP_SHARED,
        -1,                              //fd
        0                                //offset
    );
    if (chunk == MAP_FAILED) {
        printf("Error. mremap() faild. Exiting now\n");
        perror("");
        exit(EXIT_FAILURE);
    }

    printf("timing mremap...\n");
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < TEST_SIZE; i++) {
        current_length *= 2;
        chunk = mremap(
            chunk,                           //old addr
            page_size,                       //old size
            page_size*current_length,        //new size
            MREMAP_MAYMOVE
        );
        if (chunk == MAP_FAILED) {
            printf("Error. mremap() faild on iteration %i. Exiting now\n", i);
            perror("");
            exit(EXIT_FAILURE);
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec duration;
    duration.tv_nsec = end.tv_nsec - start.tv_nsec;
    duration.tv_sec = end.tv_sec - start.tv_sec;
    long int micros = duration.tv_nsec/1000;//not including seconds
    printf("done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);

    //munmap(chunk, page_size*current_length);

    current_length = 1;
    uint8_t* realloc_chunk = malloc(sizeof(uint8_t)*page_size*current_length);
    if (realloc_chunk == NULL) {
        printf("Error. malloc() faild. Exiting now\n");
        perror("");
        exit(EXIT_FAILURE);
    }

    printf("\ntiming realloc...\n");
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < TEST_SIZE; i++) {
        current_length *= 2;
        realloc_chunk = realloc(realloc_chunk, page_size*current_length);
        if (realloc_chunk == NULL) {
            printf("Error. malloc() faild on iteration %i. Exiting now\n", i);
            perror("");
            exit(EXIT_FAILURE);
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    duration.tv_nsec = end.tv_nsec - start.tv_nsec;
    duration.tv_sec = end.tv_sec - start.tv_sec;
    micros = duration.tv_nsec/1000;//not including seconds
    printf("done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);

    //free(realloc_chunk);

    printf("Done. Press enter to free memory and quit.\n");
    getc(stdin);
    munmap(chunk, page_size*current_length);
    free(realloc_chunk);
    return 0;
}
