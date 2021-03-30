
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

#define alot(of) 0xBADC0DE

//manually fetched from getrlimit(), see user_vm_space_stats.c for details
#define ARRAY_SPACE 18446744073709551615
#define ARRAY_COUNT 32000
//32000 seems to be the limit.

struct timespec diff(struct timespec start, struct timespec end);
int nth_bit(size_t num);
size_t align_to_page(size_t target_size, size_t page_size);

int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());

    size_t array_size = sqrt(18446744073709551615.0);
    printf("test size: %li\n", array_size);
    size_t page_size = getpagesize();
    size_t aligned_array_size = align_to_page(array_size, page_size);
    
    printf("%li sized maps aligned to %li, with page size of %li\n", array_size, aligned_array_size, page_size);

    printf("Timing mmap 4gb upfront...\n");
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);
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
    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec duration = diff(start, end);
    printf("Done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);

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

    printf("Press enter to free pages.\n");
    getc(stdin);
    munmap(chunk, aligned_array_size);
    printf("Press enter to quit.\n");
    getc(stdin);

    return EXIT_SUCCESS;
}

int main2(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());
    //printf("test size: %i\n", TEST_SIZE);

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
    for (int j = 0; j < page_size; j++) {
        chunk[j] = 7;
    }

    printf("Timing mremap 2n expansion...\n");
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < 1; i++) {
        size_t old_length = current_length;
        current_length *= 2;
        /*chunk = mremap(
            chunk,                           //old addr
            page_size*old_length,           //old size
            page_size*current_length,      //new size
            MREMAP_MAYMOVE
        );*/
        if (chunk == MAP_FAILED) {
            printf("Error. mremap() faild on iteration %i. Exiting now\n", i);
            perror("");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < page_size*old_length; j++) {
            chunk[(page_size*current_length-(page_size*old_length))+j] = 8;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec duration = diff(start, end);
    printf("Done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);

    //double check it actually allocated and wrote..
    printf("Checking that the stored values are valid...\n");
    for (int i = 0; i < page_size; i++) {
        if (chunk[i] != 7) {
            printf("Error... different value read back from first area: %i at index %i.\n", chunk[i], i);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = page_size; i < page_size*current_length; i++) {
        if (chunk[i] != 8) {
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

//only for positives
//returns the amount of lower zero bits in a row
int nth_bit(size_t num) {
    uint64_t n = 1;
    int res = 0;
    while (!(n & num) || res == 63) {
        n <<= 1;
        res++;
    }
    return res;
}

size_t align_to_page(size_t target_size, size_t page_size) {
    int bits_to_clear = nth_bit(page_size);
    uint64_t mask = 0xffffffffffffffff;
    uint64_t clear_mask = 1;
    bool incr = false;
    while (bits_to_clear) {
        if (target_size & clear_mask) incr = true;
        mask ^= clear_mask;
        clear_mask <<= 1;
        bits_to_clear--;
    }
    target_size &= mask;
    if (incr) target_size += page_size;
    return target_size;
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

