
#include <stdio.h>
#include <stdint.h>
//#define __USE_GNU
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "generic_vector.h"
#include "common.h"
#include "csv_append.h"
#include "self_reboot.h"

//#define TEST_SIZE 4294967000
//#define TEST_SIZE 2000000000
//#define TEST_SIZE 100000000
#define TEST_SIZE 4294967296*2
//#define WARMUP 120
//#define WARMUP 1

extern int errno;

int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());
#ifdef WARMUP
    printf("sleeping for %i seconds to let the kernel settle down.\n", WARMUP);
    sleep(WARMUP);
    printf("done sleeping.\n");
#endif
    
    printf("test size: %li\n", TEST_SIZE);

    //label the file with the start time
    struct timespec file_time;
    int res = clock_gettime(CLOCK_REALTIME, &file_time);
    clock_error_check(res);
    char* filename = malloc(sizeof(char)*(10+digits(TEST_SIZE)+digits(file_time.tv_sec)+4+1));//vector_eEEE_tTTT.csv\0
    sprintf(filename, "vector_e%li_t%i.csv", TEST_SIZE, (int)file_time.tv_sec);

    vector* vec = new_vector(0, sizeof(uint8_t));
    if (vec == NULL) {
        printf("Error. new_vector() failed. Exiting now\n");
        perror("");
        exit(EXIT_FAILURE);
    }

    //start timer
    printf("\nTiming vector expansion...\n");
    struct timespec start;
    struct timespec end;
    res = clock_gettime(CLOCK_REALTIME, &start);
    clock_error_check(res);

    //for (size_t i = 0; i <= TEST_SIZE; i++) {
    while (vec_size(vec) < TEST_SIZE) {
        uint8_t seven = 7;
        vec_push_back(vec, &seven);
    }

    //stop timer
    res = clock_gettime(CLOCK_REALTIME, &end);
    clock_error_check(res);
    struct timespec duration = diff(start, end);
    printf("Done, took %ld sec, %ld n-sec\n", duration.tv_sec, duration.tv_nsec);
    
    save_page_faults(filename);

    printf("Checking that the values are valid...\n");
    for (size_t i = 0; i < vec_size(vec); i++) {
        uint8_t* val = vec_at(vec, i);
        if (*val != 7) {
            printf("Error... different value read back from first area: %i at index %li.\n", *val, i);
            exit(EXIT_FAILURE);
        }
    }
    free_vector(vec);
    
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
