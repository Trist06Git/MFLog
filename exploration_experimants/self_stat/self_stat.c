
#include <stdio.h>
#include <stdlib.h>

#include "proc_self.h"

int main(int argc, char** argv) {
    printf("Starting.\n");

    proc_self ps;
    ps.comm = malloc(sizeof(char) * 50);
    
    int res = cat_proc_self(&ps);
    if (res == -1) return EXIT_FAILURE;

    printf("Minor page faults : %lu\n", ps.minflt);
    printf("Major page faults : %lu\n", ps.majflt);

    printf("Done.\n");
    return EXIT_SUCCESS;
}
