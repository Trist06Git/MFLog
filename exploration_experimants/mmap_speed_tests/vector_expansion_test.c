
#include <stdio.h>
#include <stdint.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "generic_vector.h"

//#define TEST_SIZE 4294967000
  #define TEST_SIZE 2000000000
#define WARMUP 120

extern int errno;

struct timespec diff(struct timespec start, struct timespec end);

int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());
#ifdef WARMUP
    printf("sleeping for %i seconds to let the kernel settle down.\n");
    sleep(WARMUP);
    printf("done sleeping.\n");
#endif
    
    printf("test size: %i\n", TEST_SIZE);

    size_t page_size = getpagesize();
    size_t current_length = 1;

    vector* vec = new_vector(0, sizeof(uint8_t));
    if (vec == NULL) {
        printf("Error. new_vector() failed. Exiting now\n");
        perror("");
        exit(EXIT_FAILURE);
    }

    printf("\nTiming vector expansion...\n");
    struct timespec start;
    struct timespec end;
    int res = clock_gettime(CLOCK_REALTIME, &start);
    if (res < 0) printf("Error. with clock_gettime().\n");

    for (long int i = 0; i < TEST_SIZE; i++) {
        uint8_t seven = 7;
        vec_push_back(vec, &seven);
    }

    res = clock_gettime(CLOCK_REALTIME, &end);
    if (res < 0) printf("Error. with clock_gettime().\n");
    struct timespec duration = diff(start, end);
    printf("Done, took %ld sec, %ld n-sec\n", duration.tv_sec, duration.tv_nsec);

    //double check it actually allocated and wrote..
    printf("Checking that the values are valid...\n");
    for (long int i = 0; i < TEST_SIZE; i++) {
        uint8_t* val = vec_at(vec, i);
        if (*val != 7) {
            printf("Error... different value read back from first area: %i at index %li.\n", *val, i);
            exit(EXIT_FAILURE);
        }
    }

    printf("Done. Press enter to free memory.\n");
    getc(stdin);
    free_vector(vec);
    printf("Press enter to quit.\n");
    getc(stdin);
    return 0;
}

//taken from stack overflow...
struct timespec diff(struct timespec start, struct timespec end) {
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}
