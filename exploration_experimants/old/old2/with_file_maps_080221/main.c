
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "mlog_alloc.h"

int main(int argc, char** argv) {
    printf("Starting.\n");
    
    bool init_res = init_arrays();
    sleep(1);

    mf_array* all_arrays = new_array(sizeof(mf_array*));
    uint limit = 2000;
    for (int i = 0; i < limit; i++) {
        mf_array* arr = new_array(sizeof(int));
        if (arr == NULL) {
            printf("Error in main, new_array...\n");
            exit(EXIT_FAILURE);
        }
        set(arr, 0, &i);
        set(all_arrays, i, &arr);
    }

    for (int i = 0; i < card(all_arrays); i++) {
        mf_array** arr = at(all_arrays, i);
        int* first = at(*arr, 0);
        if (*first != i) printf("baaad..\n");
    }

    for (int i = 0; i < 2000; i++) {
        mf_array** arr = at(all_arrays, i);
        free_array(*arr);
    }
    free_array(all_arrays);

    printf("Done.\n");
    return EXIT_SUCCESS;
}

int main0(int argc, char** argv) {
    printf("Starting.\n");
    
    mf_array* myArray = new_array(sizeof(int));
    if (myArray != NULL) {
        
        for (int i = 0; i < 3; i++) {
            if(!set(myArray, i, &i)) {
                printf("Error, set main\n");
            }
        }
        int seven = 7;
        int eight = 8;
        if (!set(myArray, -1, &seven)) printf("Error, set main\n");
        if (!set(myArray, -1, &eight)) printf("Error, set main\n");


        for (int i = 0; i < card(myArray); i++) {
            int* res = at(myArray, i);
            printf("element: %i, index: %i\n", *res, i);
        }
        
        free_array(myArray);
    } else {
        printf("Error when using new_array from main..\n");
    }
    

    printf("Done.\n");
    return EXIT_SUCCESS;
}
