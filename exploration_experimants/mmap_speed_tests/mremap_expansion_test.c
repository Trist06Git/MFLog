
#include <stdio.h>
#include <stdint.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"
#include "csv_append.h"
#include "self_reboot.h"

#define TEST_SIZE 4294967296/4096
//#define WARMUP 120

extern int errno;

//realloc vs mremap, as per manual pages who claim mremap its faster...
//and indeed it is faster.
int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());
#ifdef WARMUP
    printf("sleeping for %i seconds to let the kernel settle down.\n", WARMUP);
    sleep(WARMUP);
    printf("done sleeping.\n");
#endif

    printf("test size: %li\n", TEST_SIZE);
    size_t page_size = getpagesize();
    size_t current_length = 1;

    //label the file with the start time
    struct timespec file_time;
    int res = clock_gettime(CLOCK_REALTIME, &file_time);
    clock_error_check(res);
    char* filename = malloc(sizeof(char)*(8+digits(file_time.tv_sec)+4+1));//upfrontmmap_tXXX.csv\0
    sprintf(filename, "mremap_t%i.csv", (int)file_time.tv_sec);

    printf("Timing mremap 2n expansion...\n");
    struct timespec start;
    struct timespec end;
    //start timer
    res = clock_gettime(CLOCK_REALTIME, &start);
    clock_error_check(res);
    uint8_t* chunk = mmap(
        NULL,
        page_size*current_length,
        PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_ANONYMOUS|MAP_PRIVATE,
        -1,                               //fd
        0                                //offset
    );
    if (chunk == MAP_FAILED) {
        printf("Error. mremap() faild. Exiting now\n");
        perror("");
        exit(EXIT_FAILURE);
    }

    int max_length = TEST_SIZE;
    while (current_length <= max_length) {
        size_t old_length = current_length;
        current_length *= 2;
        chunk = mremap(
            chunk,                           //old addr
            page_size*old_length,           //old size
            page_size*current_length,      //new size
            MREMAP_MAYMOVE
        );
        if (chunk == MAP_FAILED) {
            printf("Error. mremap() failed when expanding to %li. Exiting now\n", current_length*page_size);
            perror("");
            exit(EXIT_FAILURE);
        }
    }
    
    //stop timer
    res = clock_gettime(CLOCK_REALTIME, &end);
    clock_error_check(res);
    struct timespec duration = diff(start, end);
    printf("Done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);

    //we're only concerned with the allocation and not insertion at this point,
    //so we'll save the page faults here
    save_page_faults(filename);

    printf("Checking that the addresses are valid...\n");
    for (int i = 0; i < page_size*current_length; i++) {
        chunk[i] = 7;
    }
    for (int i = 0; i < page_size*current_length; i++) {
        if (chunk[i] != 7) {
            printf("Error... different value read back from expanded area: %i at index %i.\n", chunk[i], i);
            exit(EXIT_FAILURE);
        }
    }
    munmap(chunk, page_size*current_length);
    
    //we are only interested in runs that are successful so we save its results after checking
    int temp = 0;
    vector* times = new_vector(3, sizeof(int));
    vec_push_back(times, &temp);
    vec_push_back(times, (temp = duration.tv_sec,  &temp));
    vec_push_back(times, (temp = duration.tv_nsec, &temp));

    if(!append_csv(filename, times, NULL)) {
        exit(EXIT_FAILURE);
    }
    free_vector(times);
    free(filename);

    //printf("Done. Press enter to free memory.\n");
    //getc(stdin);
    //printf("Press enter to quit.\n");
    //getc(stdin);

    self_reboot();
    printf("Done.\n");

    return EXIT_SUCCESS;
}
