
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "some_struct.h"
#include "y.tab.h"

//extern FILE* yyin;
super parsed;

int flag;

int verbose = 0;

int main(int argc, char** argv) {
    printf("Starting.\n");

    yyparse();
    if (flag == 1) {
        printf("\nInvalid expression!\n");
        return EXIT_FAILURE;
    }

    printf("parsed :\n%s : %s = %i\n", parsed.var.name, parsed.name, parsed.var.val);

    printf("Done.\n");
    return EXIT_SUCCESS;
}
