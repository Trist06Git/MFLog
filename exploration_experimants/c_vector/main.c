
#include <stdlib.h>
#include <stdio.h>

#include "int_vector.h"

int main(void) {
    printf("Starting.\n");

    int_vector* myVec = new_int_vector(0);
    for (int i = 0; i < 10; i++) {
        printf("pushing %i\n", i);
        push_back(myVec, i);
    }

    printf("Done.\n");
    return EXIT_SUCCESS;
}
