
#include <stdio.h>
#include <stdlib.h>

#include "generic_vector.h"

int main() {
    printf("Starting.\n");
    printf("size of element : %lu\n", sizeof(int));

    vector* vec = new_vector(1, sizeof(char*));

    insert_at(vec, 0, "thing");
    insert_at(vec, 0, "thing");
    
    free_vector(vec);

    printf("Done.\n");
    return EXIT_SUCCESS;
}
