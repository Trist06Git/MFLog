#ifndef INTERNAL_STRUCT_H
#define INTERNAL_STRUCT_H

#include <stdbool.h>

#include "generic_vector.h"

typedef struct atom atom;
typedef struct val val;
typedef struct var var;
typedef struct fcall fcall;
typedef struct and and;
typedef struct tuple tuple;
typedef struct equality equality;
typedef struct expr expr;
typedef struct function function;
typedef struct choice_point choice_point;

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

struct tuple {
    and n;
};

struct equality {
    expr* lhs;
    expr* rhs;
};

enum e_type {e_fcall, /*e_val, e_var,*/ e_atom, e_equ, e_and, e_tuple, e_builtin};
struct expr {
    enum e_type type;
    union {
        fcall f;
        //val vl;//probs take these out
        //var vr;//probs take these out
        atom a;
        and n;
        tuple t;
        equality e;
    } e;
};

struct function {
    char* name;
    vector* params;//as raw atoms
    expr e;
    bool fully_defined;
};

struct choice_point {
    vector* functions;//as functions
};

and append_exprs_and(vector* exprs);
and append_exprs_and_init(vector* exprs);
void append_expr(expr* nd, expr* ex);
expr* last_and(expr* nd);
bool is_var_a(atom*);
bool is_var_e(expr*);
bool is_val_a(atom*);
bool is_val_e(expr*);
bool is_and_e(expr*);
bool is_tuple_e(expr*);
bool is_fcall_e(expr*);
bool is_equ_e(expr*);
bool is_generated_var(expr*);
atom make_var_a(char*);
expr make_var_e(char*);
expr copy_var_e(expr*);

bool compare_atoms_a(atom*, atom*);

function copy_fdef(function*);
expr copy_expr(expr*);
fcall copy_fcall(fcall*);
atom copy_atom(atom*);
and copy_and(and*);
equality copy_equ(equality*);
val copy_val(val*);
var copy_var(var*);
vector* duplicate_params_a(vector* params);
vector* duplicate_params_e(vector* params);

void free_fdef(function*);
void free_expr(expr*);
void free_fcall(fcall*);
void free_atom(atom*);
void free_and(and*);
void free_equ(equality*);
void free_val(val*);
void free_var(var*);
void free_params_a(vector* params);
void free_params_e(vector* params);

#endif