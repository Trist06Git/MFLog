
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

    uint8_t* chunk = mmap(
        NULL,
        page_size*current_length,
        PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_ANONYMOUS|MAP_PRIVATE,
        -1,                              //fd
        0                                //offset
    );
    if (chunk == MAP_FAILED) {
        printf("Error. mremap() faild. Exiting now\n");
        perror("");
        exit(EXIT_FAILURE);
    }

    printf("Timing mremap 2n expansion...\n");
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < TEST_SIZE; i++) {
        size_t old_length = current_length;
        current_length *= 2;
        chunk = mremap(
            chunk,                           //old addr
            page_size*old_length,           //old size
            page_size*current_length,      //new size
            MREMAP_MAYMOVE
        );
        if (chunk == MAP_FAILED) {
            printf("Error. mremap() faild on iteration %i. Exiting now\n", i);
            perror("");
            exit(EXIT_FAILURE);
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec duration = diff(start, end);
    printf("Done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);

    //double check it actually allocated and wrote..
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

    printf("Done. Press enter to free memory.\n");
    getc(stdin);
    munmap(chunk, page_size*current_length);
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
