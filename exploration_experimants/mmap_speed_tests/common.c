
#include "common.h"

int save_page_faults(char* filecode) {
    proc_self ps;
    ps.comm = malloc(sizeof(char)*50);
    cat_proc_self(&ps);
    char* filename = malloc(sizeof(char) * (strlen(filecode) + strlen(".faults") + 1));
    sprintf(filename, "%s.faults", filecode);
    FILE* file = fopen(filename, "a+");
    if (file == NULL) {
        perror("Error :: file %s could not be opended");
        return -1;
    }
    fprintf(file, "Major faults : %lu\n", ps.majflt);
    fprintf(file, "Minor faults : %lu\n", ps.minflt);
    fclose(file);
    return 0;
}

//only for positives
int digits(int num) {
    int res = 0;
    do {res++;} while (num/=10);
    return res;
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

void clock_error_check(int res) {
    if (res < 0) {
        printf("Error. with clock_gettime().\n");
        exit(EXIT_FAILURE);
    }
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
