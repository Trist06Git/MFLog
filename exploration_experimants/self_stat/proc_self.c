
#include <stdio.h>

#include "proc_self.h"

//you must allocate a proc_self, and proc_self->comm char array yourself
//returns -1 on failure
int cat_proc_self(proc_self* ps) {
    FILE* file = fopen("/proc/self/stat", "r");
    if (file == NULL) {
        perror("Error :: Could not open /proc/self/stat\n");
        return -1;
    }
    int res = fscanf(file, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu",
        &ps->pid,
        ps->comm,
        &ps->state,
        &ps->ppid,
        &ps->pgrp,
        &ps->session,
        &ps->tty_nr,
        &ps->tpgid,
        &ps->flags,
        &ps->minflt,
        &ps->cminflt,
        &ps->majflt,
        &ps->cmajflt
    );
    if (res != 13) {
        printf("Error :: Wrong number of fields read from /proc/self/stat.\nExpected: %i Read: %i\n", 13, res);
        return -1;
    }
    fclose(file);
    return 0;
}
