
#include <stdio.h>
#include <stdint.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define TEST_SIZE 18

extern int errno;

struct timespec diff(struct timespec start, struct timespec end);

//realloc vs mremap, as per manual pages who claim mremap its faster...
//and indeed it is faster.
int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());
    printf("test size: %i\n", TEST_SIZE);

    size_t page_size = getpagesize();
    size_t current_length = 1;

    uint8_t* realloc_chunk = malloc(sizeof(uint8_t)*page_size*current_length);
    if (realloc_chunk == NULL) {
        printf("Error. malloc() faild. Exiting now\n");
        perror("");
        exit(EXIT_FAILURE);
    }
    for (int j = 0; j < page_size; j++) {
        realloc_chunk[j] = 7;
    }

    printf("\nTiming realloc 2n expansion...\n");
    struct timespec start;
    struct timespec end;
    int res = clock_gettime(CLOCK_REALTIME, &start);
    if (res < 0) printf("Error. with clock_gettime().\n");

    for (int i = 0; i < TEST_SIZE; i++) {
        size_t old_length = current_length;
        current_length *= 2;
        realloc_chunk = realloc(realloc_chunk, page_size*current_length);
        if (realloc_chunk == NULL) {
            printf("Error. malloc() faild on iteration %i. Exiting now\n", i);
            perror("");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < page_size*old_length; j++) {
            realloc_chunk[(page_size*current_length-(page_size*old_length))+j] = 8;
        }
    }

    res = clock_gettime(CLOCK_REALTIME, &end);
    if (res < 0) printf("Error. with clock_gettime().\n");
    struct timespec duration = diff(start, end);
    printf("Done, took %ld sec, %ld n-sec\n", duration.tv_sec, duration.tv_nsec);

    //double check it actually allocated and wrote..
    printf("Checking that the stored values are valid...\n");
    for (int i = 0; i < page_size; i++) {
        if (realloc_chunk[i] != 7) {
            printf("Error... different value read back from first area: %i at index %i.\n", realloc_chunk[i], i);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = page_size; i < page_size*current_length; i++) {
        if (realloc_chunk[i] != 8) {
            printf("Error... different value read back from expanded area: %i at index %i.\n", realloc_chunk[i], i);
            exit(EXIT_FAILURE);
        }
    }

    printf("Done. Press enter to free memory.\n");
    getc(stdin);
    free(realloc_chunk);
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
