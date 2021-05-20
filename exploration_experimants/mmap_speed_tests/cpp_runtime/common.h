
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "proc_self.h"

int save_page_faults(const char* filecode);
struct timespec diff(struct timespec start, struct timespec end);
int digits(long int num);
int nth_bit_common(size_t num);
void clock_error_check(int res);
size_t align_to_page_common(size_t target_size, size_t page_size);
