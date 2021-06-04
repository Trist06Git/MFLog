
//for testing
//#define TEST_COUNT 2
//#define PRE_EXPANDED

#ifndef TEST_COUNT
#error "TEST_COUNT not set."
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include "mlog_alloc_benchmark.h"

//word is 8bytes, * MEGA_WORDS = 4gb, the size of our arrays
#define MEGA_WORDS 536870912

#ifdef MREMAP_EXPANSION
char* test_name = "mremap() expansion";
#elif defined REALLOC_EXPANSION
char* test_name = "realloc() expansion";
#elif defined PRE_EXPANDED
char* test_name = "mmap() pre-expanded";
#endif

char* tick   = "\xE2\x9C\x94";
struct timespec run_test(int num);
struct timespec diff(struct timespec start, struct timespec end);
struct timespec total(struct timespec*, size_t count);
struct timespec average(struct timespec, size_t count);

int main(int argc, char** argv) {
    printf("::: Starting benchmark for %s :::\n", test_name);
    
    struct timespec times[TEST_COUNT];
    for (size_t i = 0; i < TEST_COUNT; i++) {
        times[i] = run_test(i);
    }

    struct timespec t_total   = total(times, TEST_COUNT);
    struct timespec t_average = average(t_total, TEST_COUNT);

    printf("Done all iterations.\n");
    printf("Total time : %lis, %lins\n", t_total.tv_sec, t_total.tv_nsec);
    printf("Average    : %lis, %lins\n", t_average.tv_sec, t_average.tv_nsec);

    return EXIT_SUCCESS;
}

struct timespec run_test(int num) {
    printf("::: Benchmark iteration: %i :::\n", num);

    //start of timer
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);

    mf_array* arr = new_array(sizeof(uint64_t));
    if (arr == NULL) {
        printf("Error in main, new_array...\n");
        exit(EXIT_FAILURE);
    }
    //printf("Allocated an array for %i objects of %li bytes each\n", MEGA_WORDS, sizeof(uint64_t));

    for (uint64_t i = 0; i < MEGA_WORDS; i++) {
        if(set(arr, i, &i) == false) {
            printf("Error in main, set(arr, %li, %li)\n", i, i);
            exit(EXIT_FAILURE);
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec duration = diff(start, end);
    printf("Done iteration %i, took %li sec, %li n-sec\n", num, duration.tv_sec, duration.tv_nsec);
    //end of timer

    printf("Checking array values.\n");
    for (uint64_t i = 0; i < MEGA_WORDS; i++) {
        uint64_t* val = at(arr, i);
        if (*val != i) {
            printf("Error. Wrong value in array at %li -> %li\n", i, *val);
            exit(EXIT_FAILURE);
        }
    }
    printf("Freeing pages.\n");
    free_array(arr);

    return duration;
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

struct timespec total(struct timespec* times, size_t count) {
    if (count <= 0) {
        printf("average() give size of <= 0\n");
        exit(EXIT_FAILURE);
    } 
    long second = 1000000000;
    struct timespec res;
    res.tv_nsec = times[0].tv_nsec;
    res.tv_sec  = times[0].tv_sec;
    for (size_t i = 1; i < count; i++) {
        res.tv_sec  += times[0].tv_sec;
        res.tv_nsec += times[0].tv_nsec;
        if (res.tv_nsec >= second) {
            res.tv_nsec -= second;
            res.tv_sec++;
        }
    }
    return res;
}

//loosing alot of precision
//see adding nsecs to make sec in total()
//also nsec /= count
struct timespec average(struct timespec times, size_t count) {
    struct timespec res;
    res.tv_sec  = times.tv_sec / count;
    res.tv_nsec = times.tv_nsec / count;
    return res;
}

int main_old(int argc, char** argv) {
    printf("Running benchmark.\n");

    //start of timer
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);

    mf_array* arr = new_array(sizeof(uint64_t));
    if (arr == NULL) {
        printf("Error in main, new_array...\n");
        exit(EXIT_FAILURE);
    }
    //printf("Allocated an array for %i objects of %li bytes each\n", MEGA_WORDS, sizeof(uint64_t));

    for (uint64_t i = 0; i < MEGA_WORDS; i++) {
        if(set(arr, i, &i) == false) {
            printf("Error in main, set(arr, %li, %li)\n", i, i);
            exit(EXIT_FAILURE);
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec duration = diff(start, end);
    printf("Done, took %li sec, %li n-sec\n", duration.tv_sec, duration.tv_nsec);
    //end of timer

    printf("Wrote %i objects, %li bytes each.\nPress enter to check.\n%s ", MEGA_WORDS, sizeof(uint64_t), tick);
    getc(stdin);
    for (uint64_t i = 0; i < MEGA_WORDS; i++) {
        uint64_t* val = at(arr, i);
        if (*val != i) {
            printf("Error. Wrong value in array at %li -> %li\n", i, *val);
            exit(EXIT_FAILURE);
        }
    }
    printf("Done. Press enter to free pages.\n%s ", tick);
    getc(stdin);
    free_array(arr);
    printf("Press enter to quit.\n%s ", tick);
    getc(stdin);

    return EXIT_SUCCESS;
}
