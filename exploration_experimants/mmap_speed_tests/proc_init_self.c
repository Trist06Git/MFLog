
#include <stdio.h>

#include "proc_self.h"
#include "self_reboot.h"

int main(int argc, char** argv) {
    printf("Started.\n");

    proc_self ps;
    cat_proc_self(&ps);
    printf("My pid is %i\n", ps.pid);
    self_reboot();

    return 0;
}
