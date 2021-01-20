
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "internal_struct.h"
#include "generic_vector.h"

and append_exprs_and(vector* exprs) {
    and this;
    this.lhs = malloc(sizeof(expr));
    this.rhs = malloc(sizeof(expr));
    
    memcpy(this.lhs, at(exprs, 0), sizeof(expr));
    remove_at(exprs, 0);
    if (size(exprs) == 1) {
        memcpy(this.rhs, at(exprs, 0), sizeof(expr));
    } else {
        expr next;
        next.type = e_and;
        next.e.n = append_exprs_and(exprs);
        memcpy(this.rhs, &next, sizeof(expr));
    }
    return this;
}

and append_exprs_and_init(vector* exprs) {
    if (size(exprs) < 2) {
        and res = {NULL, NULL};//really not good.. oh well.
        return res;
    } else {
        return append_exprs_and(exprs);
    }
}

//no memcpy
void append_expr(expr* nd, expr* ex) {
    expr* last = last_and(nd);
    expr new_left = *last;

    last->type = e_and;
    last->e.n.lhs = malloc(sizeof(expr));
    *last->e.n.lhs = new_left;
    last->e.n.rhs = malloc(sizeof(expr));
    *last->e.n.rhs = *ex;
}
/*//really not sure why it wouldnt work with memcpy...??
void append_expr(expr* nd, expr* ex) {
    expr* last = last_and(nd);
    expr* new_left = malloc(sizeof(expr));
    //if (is_fcall_e(*last)) printf("moving %s, with %i params\n", last->e.f.name, size(last->e.f.params));
    memcpy(new_left, last, sizeof(expr));
    last->type = e_and;
    last->e.n.lhs = new_left;
    if (is_fcall_e(*last->e.n.lhs)) printf("moved %s, with %i params\n", last->e.n.lhs->e.f.name, size(last->e.n.lhs->e.f.params));
    memcpy(last->e.n.rhs, ex, sizeof(expr));
}*/

//progress down the tree untill the last and operand
expr* last_and(expr* nd) {
    while (is_and_e(*nd)) {
        nd = nd->e.n.rhs;
    }
    return nd;
}

bool is_var_a(atom a)   { return a.type == a_var; }
bool is_var_e(expr e)   { return e.type == e_atom && is_var_a(e.e.a); }
bool is_val_a(atom a)   { return a.type == a_val; }
bool is_val_e(expr e)   { return e.type == e_atom && is_val_a(e.e.a); }
bool is_and_e(expr e)   { return e.type == e_and; }
bool is_fcall_e(expr e) { return e.type == e_fcall; }
bool is_equ_e(expr e)   { return e.type == e_equ; }
bool is_generated_var(expr e) {
    if (e.type != e_atom || e.e.a.type != a_var) return false;
    return strstr(e.e.a.data.vr.symbol, "U_") != NULL;
}

atom make_var_a(char* name) {
    atom new_var = {a_var, .data.vr = name};
    return new_var;
}

expr make_var_e(char* name) {
    expr new_var = {e_atom, .e.a = make_var_a(name)};
    return new_var;
}
