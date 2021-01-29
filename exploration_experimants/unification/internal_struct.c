
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
    while (is_and_e(nd)) {
        nd = nd->e.n.rhs;
    }
    return nd;
}

bool is_var_a(atom* a)   { return a->type == a_var; }
bool is_val_a(atom* a)   { return a->type == a_val; }
bool is_var_e(expr* e)   { return is_atom_e(e) && is_var_a(&e->e.a); }
bool is_val_e(expr* e)   { return is_atom_e(e) && is_val_a(&e->e.a); }
bool is_atom_e(expr* e)  { return e->type == e_atom;  }
bool is_and_e(expr* e)   { return e->type == e_and;   }
bool is_tuple_e(expr* e) { return e->type == e_tuple; }
bool is_fcall_e(expr* e) { return e->type == e_fcall; }
bool is_equ_e(expr* e)   { return e->type == e_equ;   }
bool is_generated_var(expr* e) {
    if (e->type != e_atom || e->e.a.type != a_var) return false;
    return strstr(e->e.a.data.vr.symbol, "U_") != NULL;
}

atom make_var_a(char* name) {
    atom new_var = {a_var, .data.vr = name};
    return new_var;
}
expr make_var_e(char* name) {
    expr new_var = {e_atom, .e.a = make_var_a(name)};
    return new_var;
}
expr make_query(atom* at) {
    expr qr;
    qr.type = e_query;
    qr.e.q = copy_atom(at);
    return qr;
}
expr wrap_atom(atom at) {
    expr ex;
    ex.type = e_atom;
    ex.e.a = at;
    return ex;
}
expr copy_var_e(expr* e) {
    char* ret_name = malloc(sizeof(char)*(strlen(e->e.a.data.vr.symbol)+1));
    strcpy(ret_name, e->e.a.data.vr.symbol);
    return make_var_e(ret_name);
}

bool compare_atoms_a(atom* a1, atom* a2) {
    if (a1->type != a2->type) {
        return false;
    } else if (a1->type == a_val) {
        if (a1->data.vl.type == a2->data.vl.type) {
            if (a1->data.vl.type == v_int) {
                return a1->data.vl.n == a2->data.vl.n;
            }//else other types
        }
    } else if (a1->type == a_var) {
        return strcmp(a1->data.vr.symbol, a2->data.vr.symbol) == 0;
    }
    return false;
}

bool compare_atoms_e(expr* a1, expr* a2) {
    if (a1->type != e_atom || a2->type != e_atom) return false;
    return compare_atoms_a(&a1->e.a, &a2->e.a);
}

function copy_fdef(function* f) {
    function ret;
    ret.name = malloc(sizeof(char)*(strlen(f->name)+1));
    strcpy(ret.name, f->name);
    ret.params = duplicate_params_a(f->params);
    ret.e = copy_expr(&f->e);
    ret.fully_defined = f->fully_defined;
    return ret;
}

expr copy_expr(expr* ex) {
    expr ret;
    ret.type = ex->type;
    if (ex->type == e_fcall) {
        ret.e.f = copy_fcall(&ex->e.f);
    } else if (ex->type == e_atom) {
        ret.e.a = copy_atom(&ex->e.a);
    } else if (ex->type == e_and) {
        ret.e.n = copy_and(&ex->e.n);
    } else if (ex->type == e_equ) {
        ret.e.e = copy_equ(&ex->e.e);
    }//add tuples
    return ret;
}

fcall copy_fcall(fcall* fc) {
    fcall ret;
    ret.name = malloc(sizeof(char)*(strlen(fc->name)+1));
    strcpy(ret.name, fc->name);
    ret.params = duplicate_params_e(fc->params);
    return ret;
}

val copy_val(val* vl) {//a little superfluous
    return *vl;
}

var copy_var(var* vr) {
    var ret;
    ret.symbol = malloc(sizeof(char)*(strlen(vr->symbol)+1));
    strcpy(ret.symbol, vr->symbol);
    return ret;
}

atom copy_atom(atom* at) {
    atom ret;
    ret.type = at->type;
    if (at->type == a_var) {
        ret.data.vr = copy_var(&at->data.vr);
    } else {
        ret.data.vl = copy_val(&at->data.vl);
    }
    return ret;
}

and copy_and(and* nd) {
    and ret;
    ret.lhs = malloc(sizeof(expr));
   *ret.lhs = copy_expr(nd->lhs);
    ret.rhs = malloc(sizeof(expr));
   *ret.rhs = copy_expr(nd->rhs);
    return ret;
}

equality copy_equ(equality* equ) {
    equality ret;
    ret.lhs = malloc(sizeof(expr));
   *ret.lhs = copy_expr(equ->lhs);
    ret.rhs = malloc(sizeof(expr));
   *ret.rhs = copy_expr(equ->rhs);
    return ret;
}

//do you promise that params is a vector of expr's?
vector* duplicate_params_e(vector* params) {
    vector* ret = new_vector(size(params), sizeof(expr));
    for (int i = 0; i < size(params); i++) {
        expr new_e = copy_expr(at(params, i));
        push_back(ret, &new_e);
    }
    return ret;
}

//do you promise that params is a vector of atoms's?
vector* duplicate_params_a(vector* params) {
    vector* ret = new_vector(size(params), sizeof(atom));
    for (int i = 0; i < size(params); i++) {
        atom new_a = copy_atom(at(params, i));
        push_back(ret, &new_a);
    }
    return ret;
}

//freeing prog tree may still have leaks and might need a closer
//look at some point
void free_fdef(function* f) {
    free(f->name);
    free_params_a(f->params);
    free_expr(&f->e);
}

void free_expr(expr* ex) {
    if (ex->type == e_fcall) {
        free_fcall(&ex->e.f);
    } else if (ex->type == e_atom) {
        free_atom(&ex->e.a);
    } else if (ex->type == e_and) {
        free_and(&ex->e.n);
    } else if (ex->type == e_equ) {
        free_equ(&ex->e.e);
    }//add tuples
}

void free_fcall(fcall* fc) {
    free(fc->name);
    free_params_e(fc->params);
}

void free_atom(atom* at) {
    if (at->type == a_var) {
        free_var(&at->data.vr);
    } else if (at->type == a_val) {
        free_val(&at->data.vl);
    }
}

void free_and(and* nd) {
    free_expr(nd->lhs);
    free_expr(nd->rhs);
    free(nd->lhs);
    free(nd->rhs);
}

void free_equ(equality* equ) {
    free_expr(equ->lhs);
    free_expr(equ->rhs);
    free(equ->lhs);
    free(equ->rhs);
}

//val is not malloced so no need to free
void free_val(val* vl) {
    return;
}

void free_var(var* vr) {
    free(vr->symbol);
}

//promise that params is atom.
void free_params_a(vector* params) {
    for (int i = 0; i < size(params); i++) {
        free_atom(at(params, i));
    }
    free_vector(params);
}

//promise that params is expr.
void free_params_e(vector* params) {
    for (int i = 0; i < size(params); i++) {
        free_expr(at(params, i));
    }
    free_vector(params);
}
