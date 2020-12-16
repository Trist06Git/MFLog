#ifndef INTERNAL_STRUCT_H
#define INTERNAL_STRUCT_H

#include "generic_vector.h"

typedef struct atom atom;
typedef struct val val;
typedef struct var var;
typedef struct fcall fcall;
typedef struct and and;
typedef struct equality equality;
typedef struct expr expr;
typedef struct function function;

struct val {
    int n;
};

struct var {
    char* symbol;
};

enum a_type {a_val, a_var};
struct atom {
    enum a_type type;
    union {
        val vl;
        var vr;
    } data;
};

struct fcall {
    char* name;
    vector* params;
};

struct and {
    expr* lhs;
    expr* rhs;
};

struct equality {
    expr* lhs;
    expr* rhs;
};

enum e_type {e_fcall, e_val, e_var, e_atom, e_equ, e_and};
struct expr {
    enum e_type type;
    union {
        fcall f;
        val vl;
        var vr;
        atom a;
        and n;
        equality e;
    } e;
};

struct function {
    char* name;
    vector* params;
    expr e;
};

#endif