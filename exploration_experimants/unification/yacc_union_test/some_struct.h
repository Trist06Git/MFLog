#ifndef SOME_STRUCT_H
#define SOME_STRUCT_H

typedef struct assign {
    char* name;
    int val;
} assign;

typedef struct super {
    char* name;
    assign var;
} super;

#endif