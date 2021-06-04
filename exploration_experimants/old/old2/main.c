
#include "mafialog_alloc.h"

int main(int argc, char** argv) {
    printf("%s\n\n", "::Starting::");
    init_sig_handles();
    
    printf("::platform specifics::\nmax open files: %i\ninitial page size: %i\nchar: %lu\nint : %lu\nlong: %lu\nunsigned long int: %lu\nsize_t: %lu\n\n",
            get_open_max(),
            getpagesize(),
            sizeof(char),
            sizeof(int),
            sizeof(long),
            sizeof(unsigned long int),
            sizeof(size_t)
    );

    array_store* myVar = new_var("myVar");
    value testVal;
    testVal.val = 88;
    testVal.tag = Value;
    for (int i = 0; i < 16; i++) {
        set(myVar, testVal, i);
    }
    debug_var(myVar);
    dump_var(myVar);

    free_var(myVar);

    printf("\n%s\n", "::Done::");
    return EXIT_SUCCESS;
}