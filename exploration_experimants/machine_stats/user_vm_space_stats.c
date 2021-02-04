
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

static inline unsigned long bytes_to_gigabytes(unsigned long bytes);
static inline double bytes_to_exabytes(double bytes);

int main(int argc, char** argv) {
    printf("Starting.\n");

    struct rlimit rlim_addr_space;
    int res = getrlimit(RLIMIT_AS, &rlim_addr_space);
    if (res < 0) {
        printf("Error, getrlimit() failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("Current address space: %lu bytes\n", rlim_addr_space.rlim_cur);
    printf("                     : %lu gigabytes\n", bytes_to_gigabytes(rlim_addr_space.rlim_cur));
    printf("                     : %.2f exabytes\n", bytes_to_exabytes(rlim_addr_space.rlim_cur));

    printf("Done.\n");
    return 0;
}

//both taken from stack overflow
static inline unsigned long bytes_to_gigabytes(unsigned long bytes) {
    return bytes / (1024 * 1024 * 1024);
}

static inline double bytes_to_exabytes(double bytes) {
    return bytes / (1024.00 * 1024.00 * 1024.00 * 1024.00 * 1024.00 * 1024.00);
}
