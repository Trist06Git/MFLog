
#include <stdio.h>
#include <stdint.h>

int main(int argc, char** argv) {
    printf("Starting.\n\n");

    printf("word size is probably : %li\n", sizeof(size_t));

    printf("void*     : %li bytes long\n", sizeof(void*));
    printf("size_t    : %li\n", sizeof(size_t));
    printf("char      : %li\n", sizeof(char));
    printf("char*     : %li\n", sizeof(char*));
    printf("int       : %li\n", sizeof(int));
    printf("int*      : %li\n", sizeof(int*));
    printf("uint64_t  : %li\n", sizeof(uint64_t));
    printf("uint64_t* : %li\n", sizeof(uint64_t*));
    printf("float     : %li\n", sizeof(float));
    printf("double    : %li\n", sizeof(double));

    printf("\nDone.\n");
    return 0;
}
