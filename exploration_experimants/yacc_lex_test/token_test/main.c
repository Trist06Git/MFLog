
#include <stdio.h>
#include <stdlib.h>

#include "y.tab.h"

int flag;

int main(int argc, char** argv) {
    printf("Started.\n");

    yyparse();
    if (flag == 1) {
        printf("\nInvalid expression!\n");
        return EXIT_FAILURE;
    }
    
    printf("Done.\n");
    return EXIT_SUCCESS;
}
