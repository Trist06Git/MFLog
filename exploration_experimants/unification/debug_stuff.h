#ifndef DEBUG_STUFF_H
#define DEBUG_STUFF_H

#include "internal_struct.h"
#include "unify.h"
#include "generic_vector.h"

void dump_func(function);
void dump_cp(choice_point*);
void dump_expr(expr, bool);
void dump_func_call(fcall);
void dump_tuple(expr* t);
void rec_dump_tuple(and*);
char* expr_to_string(expr);
char* atom_to_string(atom);
void dump_expr_vec(vector*);

#endif