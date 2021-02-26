
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
        printf("%s", atom_to_string(*(atom*)vec_at(f.params, i)));
        if(i != ps-1) printf(", ");
    }
    printf(") = ");
    //print function body
    if (f.e.type != e_builtin) {
        printf("\n");
        dump_expr(f.e, true);
        printf(".\n");
    } else {
        printf("...\n");
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
            //printf("%s%s", in, expr_to_string(e));
            printf("%s", in);
            dump_func_call(e.e.f, false);
        } break;
/*        case e_val   : {
            printf("%s%s", in, expr_to_string(e));
        } break;
        case e_var   : {
            printf("%s%s", in, expr_to_string(e));
        } break;*/
        case e_atom  : {
            printf("%s%s", in, expr_to_string(e));
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
            printf("%s%s", in, expr_to_string(e));
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
            case rs_first : printf("first answer");            break;
            case rs_one   : printf("single answer");           break;
            case rs_all   : printf("all answers");              break;
            case rs_n     : printf("answer %i", func.rs_index); break;
            default       :                                     break;
        }
    }
    
}

void dump_tuple(expr* e) {
    printf("(");
    //rec_dump_tuple(&e->e.t.n);
    vector* ands = e->e.t.n.ands;
    for (int i = 0; i < vec_size(ands); i++) {
        expr* a = vec_at(ands, i);
        dump_expr(*a, false);
        if (i < vec_size(ands)-1) printf(", ");
    }
    printf(")");
}

/*
void rec_dump_tuple(and* a) {
    dump_expr(*a->lhs, false);
    printf(",");
    if (is_and_e(a->rhs)) {
        rec_dump_tuple(&a->rhs->e.n);
    } else {
        dump_expr(*a->rhs, false);
    }
}*/

char* expr_to_string(expr e) {
    char* res;
    switch (e.type) {
        case e_fcall : {////needs cleaning up, please remove...
            fcall f = e.e.f;
            int ssize = 0;
            for (int i = 0; i < vec_size(f.params); i++) {
                expr p_i = *(expr*)vec_at(f.params, i);
                if (p_i.type == e_atom) {
                    if (p_i.e.a.type == a_val)
                        ssize += digits_neg(p_i.e.a.data.vl.n);
                    else
                        ssize += strlen(p_i.e.a.data.vr.symbol) + 2;
                } else {
                    ssize += 4;
                }
            }
            
            res = malloc(sizeof(char)*ssize+1);
            sprintf(res, "%s(", f.name);
            for (int i = 0; i < vec_size(f.params); i++) {
                expr p_i = *(expr*)vec_at(f.params, i);
                if (p_i.type == e_atom) {
                    sprintf(res, "%s%s", res, atom_to_string((*(expr*)vec_at(f.params, i)).e.a));
                } else {
                    sprintf(res, "%sex_$old$", res);
                }
                
                if (i != vec_size(f.params)-1) sprintf(res, "%s, ", res);
            }
            sprintf(res, "%s)", res);
        } return res;
        /*case e_val : {
            val v = e.e.vl;
            res = malloc(sizeof(char)*digits_neg(v.n)+1);
            sprintf(res, "%i", v.n);
        } return res;
        case e_var : {
            var v = e.e.vr;
            res = v.symbol;
        } return res;*/
        case e_atom : {
            return atom_to_string(e.e.a);
        };
        case e_equ :
        case e_and : {
            res = malloc(sizeof(char)*2);
            sprintf(res, "?");
        } return res;
        default: return NULL;
    }
}

char* atom_to_string(atom a) {
    if (a.type == a_val) {
        int num = a.data.vl.n;
        char* res = malloc(sizeof(char)+1+digits_neg(num));
        sprintf(res, "%i", num);
        return res;
    } else {//a_var
        return a.data.vr.symbol;
    }
}

void dump_expr_vec(vector* vec) {
    for (int i = 0; i < vec_size(vec); i++) {
        expr* ex = vec_at(vec, i);
        dump_expr(*ex, false); nl;
    }
}
