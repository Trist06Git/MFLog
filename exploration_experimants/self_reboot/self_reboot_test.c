
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "self_reboot.h"

int main(int argc, char** argv) {
    printf("Starting.\n");

    printf("writing test file\n");
    FILE* file = fopen("fs_sync_test_reboot.txt", "a+");
    if (file == NULL) {
        printf("error. could not open file\n");
    } else {
        fprintf(file, "file write and save successfull!!\n");
        fclose(file);
    }

    self_reboot();

    printf("Done.\n");
    return EXIT_SUCCESS;
}
