#ifndef INTERNAL_STRUCT_H
#define INTERNAL_STRUCT_H

#include <stdbool.h>

#include "generic_vector.h"
#include "mlog_array.h"

typedef struct atom atom;
typedef struct val val;
typedef struct symbol_nos symbol_nos;
typedef struct var var;
typedef struct list list;
typedef struct fcall fcall;
typedef struct and and;
typedef struct tuple tuple;
typedef struct equality equality;
typedef struct equality_chain equality_chain;
typedef struct expr expr;
typedef struct function function;
typedef struct choice_point choice_point;

//char list is a string
//char is a char casted int
//val as type v_string is alias for char list
enum v_type {v_int, v_list, v_string, v_char, v_notype};
struct list {
    enum v_type type;
    mf_array* lst;//NULL = empty list, with no type
    bool reference;
    bool has_vars;
};

struct val {
    enum v_type type;
    union {
        int i;
        list l;
    } v;
};

enum s_type {s_var, s_decomposed, s_unnamed, s_wild};
struct symbol_nos {
    enum s_type type;
    int scope;
    int num;
};

struct var {
    symbol_nos symbol;
};

enum a_type {a_val, a_var};
struct atom {
    enum a_type type;
    union {
        val vl;
        var vr;
    } data;
};

enum f_type {f_builtin, f_user};
enum rs_type {rs_all, rs_one, rs_first, rs_n};
struct fcall {
    enum f_type type;
    char* name;
    vector* params;//as expressions
    enum rs_type res_set;
    int rs_index;
};

struct and {
    vector* ands;//of exprs
};

struct tuple {
    and n;
};

struct equality {
    expr* lhs;
    expr* rhs;
};

struct equality_chain {
    vector* equs;//as exprs
};

enum e_type {e_fcall, e_atom, e_equ, e_equ_chain, e_and, e_tuple, e_builtin, e_query};
struct expr {
    enum e_type type;
    union {
        fcall f;
        atom a;
        and n;
        tuple t;
        equality e;
        equality_chain ec;
        atom q;//only used internally at unification stage for now..
    } e;
};

enum fd_type {fd_func, fd_fact};
struct function {
    char* name;
    vector* params;//as raw atoms
    expr e;
    bool fully_defined;//know arity
    enum e_type type;
};

struct choice_point {
    vector* functions;//as functions
};

//nasty
static expr debug_dummy_var = {.type = e_atom, .e.a = {.type = a_var, .data.vr = {.symbol = {.type = -1, .scope = -1, .num = -1}}}};

void append_expr(expr* nd, const expr* ex);
bool is_var_a(const atom*);
bool is_var_e(const expr*);
bool is_wild_e(const expr*);
bool is_global_var_ve(const expr*);
bool is_val_a(const atom*);
bool is_val_e(const expr*);
bool is_atom_e(const expr*);
bool is_and_e(const expr*);
bool is_tuple_e(const expr*);
bool is_list_e(const expr*);
bool is_list_a(const atom*);
bool is_non_list_e(const expr*);//val but not list
bool is_non_list_a(const atom*);//val but not list
bool is_fcall_e(const expr*);
bool is_equ_e(const expr*);
bool is_equ_chain_e(const expr*);
bool is_int_a(const atom*);
bool is_int_e(const expr*);
bool is_char_a(const atom*);
bool is_char_e(const expr*);
bool is_numeric_a(const atom*);
bool is_numeric_e(const expr*);
bool is_generated_var(const expr*);
bool is_list_instantiated_e(expr*);
int tuple_size_e(const expr*);
//atom make_var_a(char*);
//expr make_var_e(char*);
atom make_int_a(int);
expr make_int_e(int);
atom make_char_a(char);
expr make_char_e(char);
expr make_var_e(symbol_nos);
expr make_query(atom*);
expr make_list_e(bool reference);
equality alloc_sub(void);
expr wrap_atom(atom);//in an expr
expr wrap_and_e(and);//in an expr
expr wrap_and_t(and);//in a tuple

symbol_nos make_decomp_s(int scope, int args);

bool compare_lists_l(const list*, const list*);
bool compare_symbols_s(const symbol_nos*, const symbol_nos*);
bool compare_atoms_a(const atom*, const atom*);
bool compare_atoms_e(const expr*, const expr*);
bool compare_equs_e(const equality*, const equality*);

expr copy_var_e(const expr*);
function copy_fdef(const function*);
expr copy_expr(const expr*);
fcall copy_fcall(const fcall*);
atom copy_atom(const atom*);
and copy_and(const and*);
tuple copy_tuple(const tuple*);
equality copy_equ(const equality*);
equality_chain copy_equ_chain(const equality_chain*);
expr reference_list(list* lst);
val copy_val(const val*);
var copy_var(const var*);

vector* duplicate_params_a(vector* params);
vector* duplicate_params_e(vector* params);

void free_fdef(function*);
void free_expr(expr*);
void free_fcall(fcall*);
void free_atom(atom*);
void free_and(and*);
void free_equ(equality*);
void free_equ_chain(equality_chain*);
void free_val(val*);
void free_var(var*);
void free_params_a(vector* params);
void free_params_e(vector* params);

#endif