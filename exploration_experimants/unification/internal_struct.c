
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "internal_struct.h"
#include "generic_vector.h"
#include "debug_stuff.h"

void append_expr(expr* nd, const expr* ex)  {
    expr new_ex = copy_expr(ex);
    if (is_and_e(nd)) {
        vec_push_back(nd->e.n.ands, &new_ex);
    } else {
        expr old_ex = *nd;
        nd->type = e_and;
        nd->e.n.ands = new_vector(2, sizeof(expr));
        vec_push_back(nd->e.n.ands, &old_ex);
        vec_push_back(nd->e.n.ands, &new_ex);
    }
}

bool is_var_a(const atom* a)         { return a->type == a_var; }
bool is_val_a(const atom* a)         { return a->type == a_val; }
bool is_var_e(const expr* e)         { return is_atom_e(e) && is_var_a(&e->e.a); }
bool is_val_e(const expr* e)         { return is_atom_e(e) && is_val_a(&e->e.a); }
bool is_wild_e(const expr* e)        { return is_var_e(e) && e->e.a.data.vr.symbol.type == s_wild; }
bool is_global_var_ve(const expr* e) { return e->e.a.data.vr.symbol.scope == 0; }
bool is_list_e(const expr* e)        { return is_atom_e(e) && is_list_a(&e->e.a); } 
bool is_list_a(const atom* a)        { return is_val_a(a)  && a->data.vl.type == v_list; }
bool is_atom_e(const expr* e)        { return e->type == e_atom;  }
bool is_and_e(const expr* e)         { return e->type == e_and;   }
bool is_tuple_e(const expr* e)       { return e->type == e_tuple; }
bool is_fcall_e(const expr* e)       { return e->type == e_fcall; }
bool is_equ_e(const expr* e)         { return e->type == e_equ;   }
bool is_equ_chain_e(const expr* e)   { return e->type == e_equ_chain; }
bool is_int_a(const atom* a)         { return is_val_a(a) && a->data.vl.type == v_int; }
bool is_int_e(const expr* e)         { return is_atom_e(e) && is_int_a(&e->e.a); }
bool is_generated_var(const expr* e) {
    if (e->type != e_atom || e->e.a.type != a_var) return false;
    //return strstr(e->e.a.data.vr.symbol, "U_") != NULL;
    return e->e.a.data.vr.symbol.type == s_unnamed;
}

bool is_list_instantiated_e(expr* e) {
    list* lst = &e->e.a.data.vl.v.l;
    if (!lst->has_vars) return true;
    mf_array* arr = lst->lst;
    for (int i = 0; i < mfa_card(arr); i++) {
        expr* ei = mfa_at(arr, i);
        if (is_var_e(ei)) return false;
    }
    lst->has_vars = false;
    return true;
}

int tuple_size_e(const expr* e) {
    return vec_size(e->e.t.n.ands);
}

/*//may not need with new symbol enumeration
atom make_var_a(char* name) {
    atom new_var = {a_var, .data.vr = name};
    return new_var;
}
expr make_var_e(char* name) {
    expr new_var = {e_atom, .e.a = make_var_a(name)};
    return new_var;
}*/
atom make_int_a(int n) {
    atom new_int = {.type = a_val, .data.vl = {.type = v_int, .v.i = n}};
    return new_int;
}
expr make_int_e(int n) {
    expr new_int = {e_atom, .e.a = make_int_a(n)};
    return new_int;
}
expr make_var_e(symbol_nos s) {
    expr res = {
        .type = e_atom,
        .e.a = {
            .type = a_var,
            .data.vr.symbol = s
        }
    };
    return res;
}
expr make_query(atom* at) {
    expr qr;
    qr.type = e_query;
    qr.e.q = copy_atom(at);
    return qr;
}
expr make_list_e(bool reference) {
    list lst = {
        .type = v_notype,
        .reference = reference,
        .has_vars = false,
        .lst = NULL
    };
    expr ex = {
        .type = e_atom, .e.a = {
            .type = a_val, .data.vl = {
                .type = v_list, .v.l = lst
                }
        }
    };
    return ex;
}
equality alloc_sub(void) {
    substitution s = {
        .lhs = malloc(sizeof(expr)),
        .rhs = malloc(sizeof(expr))
    };
    return s;
}
expr wrap_atom(atom at) {
    expr ex = {e_atom, .e.a = at};
    return ex;
}
expr wrap_and_e(and nd) {
    expr ex = {e_and, .e.n = nd};
    return ex;
}
expr wrap_and_t(and nd) {
    expr ex = {e_tuple, .e.t.n = nd};
    return ex;
}

symbol_nos make_decomp_s(int scope, int args) {
    symbol_nos s = {
        .type  = s_decomposed,
        .scope = scope,
        .num   = args
    };
    return s;
}

bool compare_lists_l(const list* l1, const list* l2) {
    if (l1->lst == NULL && l2->lst == NULL) {
        return true;
    } else if (l1->lst == NULL || l2->lst == NULL) {
        return false;
    } else if (l1 == l2) {//same ref
        return true;
    }
    if (l1->type != l2->type
          ||
        mfa_card(l1->lst) != mfa_card(l2->lst)
    ) return false;
    
    for (int i = 0; i < mfa_card(l1->lst); i++) {
        expr* el1 = mfa_at(l1->lst, i);
        expr* el2 = mfa_at(l2->lst, i);
        if (compare_atoms_e(el1, el2) == false) return false;
    }
    return true;
}

bool compare_symbols_s(const symbol_nos* a, const symbol_nos* b) {
    return
      a->type  == b->type &&
      a->scope == b->scope &&
      a->num   == b->num
    ;
}

bool compare_atoms_a(const atom* a1, const atom* a2) {
    if (a1->type != a2->type) {
        return false;
    } else if (a1->type == a_val) {
        if (a1->data.vl.type == a2->data.vl.type) {
            if (a1->data.vl.type == v_int) {
                return a1->data.vl.v.i == a2->data.vl.v.i;
            } else if (a1->data.vl.type == v_list) {
                return compare_lists_l(&a1->data.vl.v.l, &a2->data.vl.v.l);
            }//else other types
        }
    } else if (a1->type == a_var && a2->type == a_var) {
        return compare_symbols_s(&a1->data.vr.symbol, &a2->data.vr.symbol);
    }
    return false;
}

bool compare_atoms_e(const expr* a1, const expr* a2) {
    if (a1->type != e_atom || a2->type != e_atom) return false;
    return compare_atoms_a(&a1->e.a, &a2->e.a);
}

//assumes that vars are on the left
bool compare_equs_e(const equality* equ_a, const equality* equ_b) {
    return
      compare_atoms_e(equ_a->lhs, equ_b->lhs) &&
      compare_atoms_e(equ_a->rhs, equ_b->rhs)
    ;
}

expr copy_var_e(const expr* e) {
    atom at;
    at.type = a_var;
    at.data.vr.symbol = e->e.a.data.vr.symbol;
    expr res = {.type = e_atom, .e.a = at};
    
    //char* ret_name = malloc(sizeof(char)*(strlen(e->e.a.data.vr.symbol)+1));
    //strcpy(ret_name, e->e.a.data.vr.symbol);
    //return make_var_e(ret_name);
    return res;
}

function copy_fdef(const function* f) {
    function ret;
    ret.name = malloc(sizeof(char)*(strlen(f->name)+1));
    strcpy(ret.name, f->name);
    ret.params = duplicate_params_a(f->params);
    ret.e = copy_expr(&f->e);
    ret.fully_defined = f->fully_defined;
    return ret;
}

expr copy_expr(const expr* ex) {
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
    } else if (ex->type == e_equ_chain) {
        ret.e.ec = copy_equ_chain(&ex->e.ec);
    } else if (ex->type == e_tuple) {
        ret.e.t = copy_tuple(&ex->e.t);
    } else {
        printf("Internal :: Error. In copy_expr(). Unknown type.\n");
    }
    return ret;
}

fcall copy_fcall(const fcall* fc) {
    fcall ret = *fc;
    ret.name = malloc(sizeof(char)*(strlen(fc->name)+1));
    strcpy(ret.name, fc->name);
    ret.params = duplicate_params_e(fc->params);
    ret.res_set = fc->res_set;
    ret.type = fc->type;
    return ret;
}

//just shallow copying done
expr reference_list(const list* lst) {
    expr ret = {
        .type = e_atom,
        .e.a = {
            .type = a_val,
            .data.vl = {
                .type = v_list,
                .v.l = *lst
            }
        }
    };
    ret.e.a.data.vl.v.l.reference = true;
    return ret;
}

val copy_val(const val* vl) {
    val ret = *vl;
    if (vl->type == v_int) {
        ret.v.i = vl->v.i;
        ret.type = vl->type;
    } else if  (vl->type == v_list) {
        //for now always copying, need to add referencing for non-mutable stuff
        //cow/copy-on-write
        if (vl->v.l.lst != NULL) {
            const list* lst = &vl->v.l;
            if (lst->reference) {
                ret.v.l.lst = lst->lst;
            } else {
                ret.v.l.lst = mfa_duplicate(vl->v.l.lst);
            }
        } else {
            /////WARNING, referencing an empty list is not possible at this time..
            if (vl->v.l.reference) printf("Warning! :: Referencing empty lists is not possible at this time.\n");
            ret.v.l.lst = NULL;
        }
        ret.v.l.type = vl->v.l.type;
        ret.v.l.has_vars = ret.v.l.has_vars;
    } else {
        printf("Error. In copy_val(). Unknown type.\n");
    }
    return ret;
}

var copy_var(const var* vr) {
    var ret;
    ret.symbol.type  = vr->symbol.type;
    ret.symbol.scope = vr->symbol.scope;
    ret.symbol.num   = vr->symbol.num;
    
    return ret;
}

atom copy_atom(const atom* at) {
    atom ret;
    ret.type = at->type;
    if (at->type == a_var) {
        ret.data.vr = copy_var(&at->data.vr);
    } else {
        ret.data.vl = copy_val(&at->data.vl);
    }
    return ret;
}

and copy_and(const and* nd) {
    and ret = {.ands = duplicate_vector(nd->ands)};
    return ret;
}

tuple copy_tuple(const tuple* tu) {
    tuple ret;
    ret.n = copy_and(&tu->n);
    return ret;
}

equality copy_equ(const equality* equ) {
    equality ret;
    ret.lhs = malloc(sizeof(expr));
   *ret.lhs = copy_expr(equ->lhs);
    ret.rhs = malloc(sizeof(expr));
   *ret.rhs = copy_expr(equ->rhs);
    return ret;
}

equality_chain copy_equ_chain(const equality_chain* equ) {
    equality_chain ret = {.equs = duplicate_vector(equ->equs)};
    return ret;
}

//do you promise that params is a vector of expr's?
vector* duplicate_params_e(vector* params) {
    vector* ret = new_vector(vec_size(params), sizeof(expr));
    for (int i = 0; i < vec_size(params); i++) {
        expr new_e = copy_expr(vec_at(params, i));
        vec_push_back(ret, &new_e);
    }
    return ret;
}

//do you promise that params is a vector of atoms's?
vector* duplicate_params_a(vector* params) {
    vector* ret = new_vector(vec_size(params), sizeof(atom));
    for (int i = 0; i < vec_size(params); i++) {
        atom new_a = copy_atom(vec_at(params, i));
        vec_push_back(ret, &new_a);
    }
    return ret;
}

//freeing prog tree may still have leaks and might need a closer
//look at some point
void free_fdef(function* f) {
    free(f->name);
    free_params_a(f->params);
    free_expr(&f->e);
    //free(e)??
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
    } else if (ex->type == e_equ_chain) {
        free_equ_chain(&ex->e.ec);
    } else if (ex->type == e_tuple) {
        //add tuples
    } else if (ex->type == e_builtin) {
        return;
    }
}

void free_fcall(fcall* fc) {
    if (fc->type == f_builtin) return;
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
    free_vector(nd->ands);
}

void free_equ(equality* equ) {
    free_expr(equ->lhs);
    free_expr(equ->rhs);
    free(equ->lhs);
    free(equ->rhs);
}

void free_equ_chain(equality_chain* equ) {
    free_vector(equ->equs);
}

void free_val(val* vl) {
    if (vl->type == v_int) {
        return;
    } else if (vl->type == v_list && vl->v.l.lst != NULL) {
        if (!vl->v.l.reference) free_mfarray(vl->v.l.lst);
        ////NOT GOOD, need a ref count...
        return;
    }
}

void free_var(var* vr) {
    //free(vr->symbol);
    return;
}

//promise that params is atom.
void free_params_a(vector* params) {
    for (int i = 0; i < vec_size(params); i++) {
        free_atom(vec_at(params, i));
    }
    free_vector(params);
}

//promise that params is expr.
void free_params_e(vector* params) {
    for (int i = 0; i < vec_size(params); i++) {
        free_expr(vec_at(params, i));
    }
    free_vector(params);
}
