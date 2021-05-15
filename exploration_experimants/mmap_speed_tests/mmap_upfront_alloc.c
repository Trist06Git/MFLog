
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"
#include "csv_append.h"
#include "self_reboot.h"

//manually fetched from getrlimit(), see user_vm_space_stats.c for details
//#define ARRAY_SPACE 18446744073709551615

//#define WARMUP 1

int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());
#ifdef WARMUP
    printf("sleeping for %i seconds to let the kernel settle down.\n", WARMUP);
    sleep(WARMUP);
    printf("done sleeping.\n");
#endif

    size_t array_size = sqrt(18446744073709551615.0);
    printf("test size: %li\n", array_size);
    size_t page_size = getpagesize();
    size_t aligned_array_size = align_to_page(array_size, page_size);
    
    printf("%li sized maps aligned to %li, with page size of %li\n", array_size, aligned_array_size, page_size);

    //label the file with the start time
    struct timespec file_time;
    int res = clock_gettime(CLOCK_REALTIME, &file_time);
    clock_error_check(res);
    char* filename = malloc(sizeof(char)*(13+digits(file_time.tv_sec)+4+1));//upfrontmmap_tXXX.csv\0
    sprintf(filename, "upfrontmmap_t%i.csv", (int)file_time.tv_sec);

    printf("Timing mmap 4gb upfront...\n");
    struct timespec start;
    struct timespec end;
    //start clock
    res = clock_gettime(CLOCK_REALTIME, &start);
    clock_error_check(res);
    uint8_t* chunk = mmap(
        NULL,
        aligned_array_size,
        PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_ANONYMOUS|MAP_PRIVATE,
        -1,                              //fd
        0                                //offset
    );
    if (chunk == MAP_FAILED) {
        printf("Error. mmap() faild. Exiting now\n");
        perror("");
        exit(EXIT_FAILURE);
    }

    //stop clock
    res = clock_gettime(CLOCK_REALTIME, &end);
    clock_error_check(res);
    struct timespec duration = diff(start, end);
    printf("Done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);

    //we're only concerned with the allocation and not insertion at this point,
    //so we'll save the page faults here
    save_page_faults(filename);

    printf("Checking that the addresses are valid...\n");
    for (size_t i = 0; i < aligned_array_size; i++) {
        chunk[i] = 7;
    }
    for (size_t i = 0; i < aligned_array_size; i++) {
        if (chunk[i] != 7) {
            printf("Error... different value read back from first area: %i at index %li.\n", chunk[i], i);
            exit(EXIT_FAILURE);
        }
    }
    munmap(chunk, aligned_array_size);
    
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

    //printf("Press enter to free pages.\n");
    //getc(stdin);    
    //printf("Press enter to quit.\n");
    //getc(stdin);

    self_reboot();
    printf("Done.\n");

    return EXIT_SUCCESS;
}
