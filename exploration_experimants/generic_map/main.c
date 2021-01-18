
#include <stdlib.h>
#include <stdio.h>

#include "generic_map.h"

int main(void) {
    printf("Starting.\n");

    vector* myVec = new_vector(0, sizeof(int));
    myVec->type = "int";
    for (int i = 0; i < 10; i++) {
        printf("pushing %i\n", i);
        push_back(myVec, &i);
    }

    printf("vecs type is %s\n", myVec->type);
    for (int i = 0; i < size(myVec); i++) {
        printf("fetched %i from vec\n", *(int*)at(myVec, i));
    }

    printf("Done.\n");
    return EXIT_SUCCESS;
}
