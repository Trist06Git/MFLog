#ifndef DEBUG_STUFF_H
#define DEBUG_STUFF_H

#include "internal_struct.h"
#include "unify.h"
#include "generic_vector.h"

void dump_func(function);
void dump_cp(choice_point*);
void dump_expr(expr, bool);
void dump_func_call(fcall, bool);
void dump_tuple(expr*);
void dump_list(list*, bool with_meta);
void dump_val(val*);
void dump_atom(atom*);
void dump_symbol(symbol_nos);
void rec_dump_tuple(and*);
void dump_expr_vec(vector*);

#endif