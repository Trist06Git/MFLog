
#include <stdio.h>
#include <stdint.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());

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

    uint8_t* middle = mmap(
        NULL,
        page_size,
        PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_ANONYMOUS|MAP_SHARED,
        -1,                              //fd
        0                                //offset
    );
    for (int i = 0; i < page_size; i++) {
        middle[i] = 8;
    }

    printf("timing mremap...\n");
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);
    //munmap(chunk+page_size, page_size);
    uint8_t* new_middle = mremap(
        middle,                          //old addr
        page_size,                       //old size
        page_size,                       //new size
        MREMAP_FIXED|MREMAP_MAYMOVE,
        chunk+page_size                  //new addr
    );
    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec duration;
    duration.tv_nsec = end.tv_nsec - start.tv_nsec;
    duration.tv_sec = end.tv_sec - start.tv_sec;
    long int micros = duration.tv_nsec/1000;//not including seconds
    printf("done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);
    
    /*
    printf("old middle  : %p\n", middle);
    printf("new middle  : %p\n", new_middle);
    printf("chunk start : %p\n", chunk);
    printf("chunk middle: %p\n", chunk+page_size);
    printf("chunk end   : %p\n", chunk+page_size*2);
    printf("chunk last  : %p\n", chunk+page_size*3);

    printf("Press enter to continue.\n");
    getc(stdin);*/

    int bad_count = 0;
    for (int i = 0; i < page_size*3; i++) {
        if (i < page_size || i >= page_size*2) {
            if (chunk[i] != 7) bad_count++;
        } else {
            if (chunk[i] != 8) bad_count++;
        }
    }
    printf("mremap bad count: %i\n", bad_count);

    uint8_t* memcpy_chunk = malloc(sizeof(uint8_t)*page_size*3);
    for (int i = 0; i < page_size*3; i++) {
        memcpy_chunk[i] = 7;
    }

    uint8_t* memcpy_middle = malloc(sizeof(uint8_t)*page_size);
    for (int i = 0; i < page_size; i++) {
        memcpy_middle[i] = 8;
    }

    printf("\ntiming memcpy...\n");
    clock_gettime(CLOCK_REALTIME, &start);

    memcpy(memcpy_chunk+page_size, memcpy_middle, page_size);
    free(memcpy_middle);

    clock_gettime(CLOCK_REALTIME, &end);
    duration.tv_nsec = end.tv_nsec - start.tv_nsec;
    duration.tv_sec = end.tv_sec - start.tv_sec;
    micros = duration.tv_nsec/1000;//not including seconds
    printf("done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);

    bad_count = 0;
    for (int i = 0; i < page_size*3; i++) {
        if (i < page_size || i >= page_size*2) {
            if (chunk[i] != 7) bad_count++;
        } else {
            if (chunk[i] != 8) bad_count++;
        }
    }
    printf("memcpy bad count: %i\n", bad_count);

    printf("Done.\n");
    return 0;
}
