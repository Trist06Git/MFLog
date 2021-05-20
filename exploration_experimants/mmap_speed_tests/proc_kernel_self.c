
#include <stdio.h>

#include "proc_self.h"
#include "self_reboot.h"

int main(int argc, char** argv) {
    printf("Started.\n");

    init_procfs();
    proc_self ps;
    ps.comm = malloc(100);
    cat_proc_self(&ps);
    printf("My pid is %i\n", ps.pid);
    self_reboot();

    return 0;
}
