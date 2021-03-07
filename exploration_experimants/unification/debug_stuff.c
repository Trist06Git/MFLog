
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "debug_stuff.h"
#include "utils.h"

#define nl printf("\n")

//all these debugging fuctions feature tons of string leaks.
void dump_func(function f) {
    //print function head
    printf("%s(", f.name);
    int ps = vec_size(f.params);
    for (int i = 0; i < ps; i++) {
        atom* ai = vec_at(f.params, i);
        dump_atom(ai);
        if(i != ps-1) printf(", ");
    }

    //print function body
    if (f.type == fd_fact) {
        printf(").\n");
    } else if (f.e.type != e_builtin) {
        printf(") = \n");
        dump_expr(f.e, true);
        printf(".\n");
    } else {
        printf(") = ...\n");
    }
}

void dump_cp(choice_point* fs) {
    for (int i = 0; i < vec_size(fs->functions); i++) {
        function* f = vec_at(fs->functions, i);
        dump_func(*f);
    }
    nl;
}

void dump_expr(expr e, bool indent) {
    char* in = indent ? "  " : "";
    switch (e.type) {
        case e_fcall : {
            printf("%s", in);
            dump_func_call(e.e.f, false);
        } break;
        case e_atom  : {
            printf("%s", in);
            dump_atom(&e.e.a);
        } break;
        case e_equ   : {
            expr e1 = *e.e.e.lhs;
            expr e2 = *e.e.e.rhs;
            dump_expr(e1, indent);
            printf(" = ");
            dump_expr(e2, indent);
        } break;
        case e_equ_chain : {
            vector* equs = e.e.ec.equs;
            for (int i = 0; i < vec_size(equs); i++) {
                expr* ei = vec_at(equs, i);
                dump_expr(*ei, i == 0 ? indent : false);
                if (i != vec_size(equs)-1) printf(" = ");
            }
            printf(" <-- chain");
        } break;
        case e_and   : {
            vector* ands = e.e.n.ands;
            for (int i = 0; i < vec_size(ands); i++) {
                expr* ai = vec_at(ands, i);
                dump_expr(*ai, indent);
                if (i != vec_size(ands)-1) printf(",\n");
            }
        } break;
        case e_tuple : {
            dump_tuple(&e);
        } break;
        case e_query : {
            printf("%s%s", in, "?query?");
        }
        default: return;
    }
}

void dump_func_call(fcall func, bool extended) {
    printf("%s(", func.name);
    for (int i = 0; i < vec_size(func.params); i++) {
        dump_expr(*(expr*)vec_at(func.params, i), false);
        if (i < vec_size(func.params)-1) printf(", ");
    }
    printf(")");
    if (1) {
        printf(" : ");
        switch (func.type) {
            case f_builtin : printf("builtin : "); break;
            case f_user    : printf("user : ");    break;
            default        :                       break;
        }
        switch (func.res_set) {
            case rs_first : printf("first answer");             break;
            case rs_one   : printf("single answer");            break;
            case rs_all   : printf("all answers");              break;
            case rs_n     : printf("answer %i", func.rs_index); break;
            default       :                                     break;
        }
    }
}

void dump_tuple(expr* e) {
    printf("(");
    vector* ands = e->e.t.n.ands;
    for (int i = 0; i < vec_size(ands); i++) {
        expr* a = vec_at(ands, i);
        dump_expr(*a, false);
        if (i < vec_size(ands)-1) printf(", ");
    }
    printf(")");
}

void dump_list(list* l) {
    printf("[");
    if (l->lst != NULL) for (int i = 0; i < mfa_card(l->lst); i++) {
        expr* ei = mfa_at(l->lst, i);
        dump_expr(*ei, false);
        if (i < mfa_card(l->lst)-1) printf(";");
    }
    printf("]");
    if (l->has_vars) printf("v");
    else             printf("i");
}

void dump_val(val* vl) {
    if (vl->type == v_int) {
        printf("%i", vl->v.i);
    } else if (vl->type == v_list) {
        dump_list(&vl->v.l);
    }
}

void dump_atom(atom* a) {
    if (is_var_a(a)) {
        printf("%s", a->data.vr.symbol);
    } else if (is_val_a(a)) {
        dump_val(&a->data.vl);
    }
}

void dump_expr_vec(vector* vec) {
    for (int i = 0; i < vec_size(vec); i++) {
        expr* ex = vec_at(vec, i);
        dump_expr(*ex, false); nl;
    }
}
