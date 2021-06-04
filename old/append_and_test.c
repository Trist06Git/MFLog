
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "internal_struct.h"
#include "generic_vector.h"
#include "utils.h"

#define nl printf("\n")

void dump_expr(expr e, bool indent);
void dump_func_call(fcall func);
char* expr_to_string(expr e);
char* atom_to_string(atom a);

int main(int argc, char** argv) {
    printf("Starting.\n");
    
    //char* y_name = malloc(sizeof(char)*2); y_name[0] = 'Y'; y_name[1] = '\0';
    //char* x_name = malloc(sizeof(char)*2); x_name[0] = 'X'; x_name[1] = '\0';
    char* y_name = "Y";
    char* x_name = "X";

    function* f = malloc(sizeof(function));
    f->name = "f";
    f->params = new_vector(0, sizeof(atom));
    f->e = make_var_e(x_name);

    expr new_ex = make_var_e(y_name);
    append_expr(&(f->e), &new_ex);

    dump_expr(f->e, false);nl;

    printf("Done.\n");
    return 0;
}

void dump_expr(expr e, bool indent) {
    char* in = indent ? "  " : "";
    switch (e.type) {
        case e_fcall : {
            //printf("%s%s", in, expr_to_string(e));
            printf("%s", in);
            dump_func_call(e.e.f);
        } break;
        case e_val   : {
            printf("%s%s", in, expr_to_string(e));
        } break;
        case e_var   : {
            printf("%s%s", in, expr_to_string(e));
        } break;
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
        case e_and   : {
            expr e1 = *e.e.n.lhs;
            expr e2 = *e.e.n.rhs;
            dump_expr(e1, indent);
            printf(",\n");
            dump_expr(e2, indent);
        } break;
        default: return;
    }
}

void dump_func_call(fcall func) {
    printf("%s(", func.name);
    for (int i = 0; i < size(func.params); i++) {
        dump_expr(*(expr*)at(func.params, i), false);
        if (i < size(func.params)-1) printf(", ");
    }
    printf(")");
}

char* expr_to_string(expr e) {
    char* res;
    switch (e.type) {
        case e_fcall : {////needs cleaning up, please remove...
            fcall f = e.e.f;
            int ssize = 0;
            for (int i = 0; i < size(f.params); i++) {
                expr p_i = *(expr*)at(f.params, i);
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
            for (int i = 0; i < size(f.params); i++) {
                expr p_i = *(expr*)at(f.params, i);
                if (p_i.type == e_atom) {
                    sprintf(res, "%s%s", res, atom_to_string((*(expr*)at(f.params, i)).e.a));
                } else {
                    sprintf(res, "%sex_", res);
                }
                
                if (i != size(f.params)-1) sprintf(res, "%s, ", res);
            }
            sprintf(res, "%s)", res);
        } return res;
        case e_val : {
            val v = e.e.vl;
            res = malloc(sizeof(char)*digits_neg(v.n)+1);
            sprintf(res, "%i", v.n);
        } return res;
        case e_var : {
            var v = e.e.vr;
            res = v.symbol;
        } return res;
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
