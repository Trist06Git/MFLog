
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "internal_struct.h"
#include "generic_vector.h"
#include "utils.h"
#include "y.tab.h"

vector* func_defs;

//builtins
fcall fc_div;
fcall fc_mul;
fcall fc_plus;
fcall fc_minus;

int flag;
int verbose = 0;

#define nl printf("\n")

void init(void);

void dump_func(function);
void dump_expr(expr, bool);
char* expr_to_string(expr);
char* atom_to_string(atom);

int main(int argc, char** argv) {
    printf("Starting.\n");

    init();
    yyparse();

    for (int i = 0; i < size(func_defs); i++) {
        dump_func(*(function*)(at(func_defs, 0)));
        nl;
    }
    
    printf("Done.\n");
    return EXIT_SUCCESS;
}

void init(void) {
    func_defs = new_vector(0, sizeof(function));
    fc_div.name   = "div";
    fc_mul.name   = "mul";
    fc_plus.name  = "plus";
    fc_minus.name = "minus";
}

void dump_func(function f) {
    //print function head
    printf("%s(", f.name);
    int ps = size(f.params);
    for (int i = 0; i < ps; i++) {
        printf("%s", atom_to_string(*(atom*)at(f.params, i)));
        if(i != ps-1) printf(", ");
    }
    printf(") = \n");
    //print function body
    //printf("  ");
    dump_expr(f.e, true);
    printf(".\n");
}

void dump_expr(expr e, bool indent) {
    char* in = indent ? "  " : "";
    switch (e.type) {
        case e_fcall : {
            printf("%s%s", in, expr_to_string(e));
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

char* expr_to_string(expr e) {
    char* res;
    switch (e.type) {
        case e_fcall : {
            fcall f = e.e.f;
            int ssize = 0;
            for (int i = 0; i < size(f.params); i++) {
                atom p_i = *(atom*)at(f.params, i);
                if (p_i.type == a_val)
                    ssize += digits_neg(p_i.data.vl.n);
                else
                    ssize += strlen(p_i.data.vr.symbol) + 2;
            }
            
            res = malloc(sizeof(char)*ssize+1);
            sprintf(res, "%s(", f.name);
            for (int i = 0; i < size(f.params); i++) {
                sprintf(res, "%s%s", res, atom_to_string(*(atom*)at(f.params, i)));
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
