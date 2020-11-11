
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define nl printf("\n")

int main(void) {
    printf("Starting.\n");
    
    printf("Data type sizes ::\n");
    printf("boolean       : %li\n", sizeof(bool));
    printf("char          : %li\n", sizeof(char));
    printf("short int     : %li\n", sizeof(short int));
    printf("int           : %li\n", sizeof(int));
    printf("long int      : %li\n", sizeof(long int));
    printf("long long int : %li\n", sizeof(long long int));
    printf("pointer       : %li\n", sizeof(char*));
    nl;

    printf("Done.\n");
    return 0;
}
