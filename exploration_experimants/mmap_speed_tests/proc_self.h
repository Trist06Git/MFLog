#ifndef PROC_SELF_H
#define PROC_SELF_H

typedef struct proc_self proc_self;

struct proc_self {
    int pid;
    char* comm;
    char state;
    int ppid;
    int pgrp;
    int session;
    int tty_nr;
    int tpgid;
    unsigned flags;
    unsigned long minflt;
    unsigned long cminflt;
    unsigned long majflt;
    unsigned long cmajflt;
};

int cat_proc_self(proc_self*);

#endif