
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nl printf("\n")

int main(int argc, char** argv) {
    printf("Starting.\n");
    
    int* mem = malloc(sizeof(int)*4);
    for (int i = 0; i < 4; i++) {
        *(mem+i) = i;
    }
    printf("before : ");
    for (int i = 0; i < 4; i++) {
        printf("%i", *(mem+i));
        if (i < 3) printf(",");
    }
    nl;

    memcpy(mem+1, mem+2, sizeof(int));
    printf("after : ");
    for (int i = 0; i < 4; i++) {
        printf("%i", *(mem+i));
        if (i < 3) printf(",");
    }
    nl;
    free(mem);

    printf("Done.\n");
    return 0;
}
