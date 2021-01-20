#ifndef INTERNAL_STRUCT_H
#define INTERNAL_STRUCT_H

#include <stdbool.h>

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
    vector* params;//as expressions
};

struct and {
    expr* lhs;
    expr* rhs;
};

struct equality {
    expr* lhs;
    expr* rhs;
};

enum e_type {e_fcall, e_val, e_var, e_atom, e_equ, e_and, e_builtin};
struct expr {
    enum e_type type;
    union {
        fcall f;
        val vl;//probs take these out
        var vr;//probs take these out
        atom a;
        and n;
        equality e;
    } e;
};

struct function {
    char* name;
    vector* params;//as raw atoms
    expr e;
    bool fully_defined;
};

and append_exprs_and(vector* exprs);
and append_exprs_and_init(vector* exprs);
void append_expr(expr* nd, expr* ex);
expr* last_and(expr* nd);
bool is_var_a(atom);
bool is_var_e(expr);
bool is_val_a(atom);
bool is_val_e(expr);
bool is_and_e(expr);
bool is_fcall_e(expr);
bool is_equ_e(expr);
atom make_var_a(char*);
expr make_var_e(char*);
bool is_generated_var(expr e);

#endif