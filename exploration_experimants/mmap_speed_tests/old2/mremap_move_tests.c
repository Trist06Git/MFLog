
#include <stdio.h>
#include <stdint.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define TEST_SIZE 1000

int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());
    printf("test size: %i\n", TEST_SIZE);

    size_t page_size = getpagesize();
    
    uint8_t* chunk = mmap(
        NULL,
        page_size*3,
        PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_ANONYMOUS|MAP_SHARED,
        -1,                              //fd
        0                                //offset
    );
    for (int i = 0; i < page_size*3; i++) {
        chunk[i] = 7;
    }

    uint8_t** to_remap = malloc(sizeof(uint8_t*)*TEST_SIZE);
    for (int i = 0; i < TEST_SIZE; i++) {
        to_remap[i] = mmap(
            NULL,
            page_size,
            PROT_READ|PROT_WRITE|PROT_EXEC,
            MAP_ANONYMOUS|MAP_SHARED,
            -1,                              //fd
            0                                //offset
        );
        for (int j = 0; j < page_size; j++) {
            to_remap[i][j] = 8;
        }
    }

    printf("timing mremap...\n");
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < TEST_SIZE; i++) {
        uint8_t* new_middle = mremap(
            to_remap[i],                     //old addr
            page_size,                       //old size
            page_size,                       //new size
            MREMAP_FIXED|MREMAP_MAYMOVE,
            chunk+page_size                  //new addr
        );
        if (new_middle != chunk+page_size) {
            printf("Error. Mapping %i has been moved to the wrong location.\n", i);
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec duration;
    duration.tv_nsec = end.tv_nsec - start.tv_nsec;
    duration.tv_sec = end.tv_sec - start.tv_sec;
    long int micros = duration.tv_nsec/1000;//not including seconds
    printf("done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);

    munmap(chunk, page_size*3);

    uint8_t** to_memcpy = malloc(sizeof(uint8_t*)*TEST_SIZE);
    for (int i = 0; i < TEST_SIZE; i++) {
        to_memcpy[i] = malloc(sizeof(uint8_t)*page_size);
        for (int j = 0; j < page_size; j++) {
            to_memcpy[i][j] = 8;
        }
    }

    uint8_t* memcpy_chunk = malloc(sizeof(uint8_t)*page_size*3);
    for (int i = 0; i < page_size*3; i++) {
        memcpy_chunk[i] = 7;
    }

    printf("\ntiming memcpy...\n");
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < TEST_SIZE; i++) {
        memcpy(memcpy_chunk+page_size, to_memcpy[i], page_size);
        free(to_memcpy[i]);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    duration.tv_nsec = end.tv_nsec - start.tv_nsec;
    duration.tv_sec = end.tv_sec - start.tv_sec;
    micros = duration.tv_nsec/1000;//not including seconds
    printf("done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);

    printf("Done.\n");
    return 0;
}
