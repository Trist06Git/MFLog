
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "internal_struct.h"
#include "explication.h"
#include "generic_vector.h"
#include "generic_map.h"
#include "utils.h"
#include "y.tab.h"

#define nl printf("\n")

int extern errno;
extern FILE* yyin;

int flag;
int verbose = 0;

vector* func_defs;

//builtins
fcall fc_div;
fcall fc_mul;
fcall fc_plus;
fcall fc_minus;
function f_div;
function f_mul;
function f_plus;
function f_minus;

void init(void);

bool functor_arity_exists(vector* defs, fcall*);

void dump_func(function);
void dump_expr(expr, bool);
void dump_func_call(fcall);
void dump_tuple(expr* t);
void rec_dump_tuple(and*);
char* expr_to_string(expr);
char* atom_to_string(atom);
void dump_expr_vec(vector*);

void print_help(void);

int main(int argc, char** argv) {
    char* filename_path = NULL;
    int option;
    while ((option = getopt(argc, argv, "hvf:")) != -1) {
        switch (option) {
            case 'h': print_help(); return EXIT_SUCCESS;       break;
            case 'v': verbose++;                               break;
            case 'f': filename_path = optarg;                  break;
            case '?': printf("Unknown option : %c\n", optopt); break;
            default :                                          break;
        }
    }
    if (verbose > 0) printf("Starting.\n");
    if (optind < argc) filename_path = argv[argc-1];
    if (filename_path != NULL) {
        /*if (verbose > 0)*/ printf("Loading file : %s\n", filename_path);
        FILE* file = fopen(filename_path, "r");
        if (file == NULL) {
            printf("Error, could not open file %s:\n  %s\n", filename_path, strerror(errno));
            return EXIT_FAILURE;
        }
        yyin = file;
        //WARN. file not closed yet.
    } else /*if (verbose > 0)*/ {
        printf("Entering interactive mode\n");
    }

    printf("Initialising.\n");
    init();
    printf("Parsing/Lexing.\n");
    yyparse();
    //close file here?

    printf("Explicating constants/variables.\n");
    for (int i = 0; i < size(func_defs); i++) {
        function* f = at(func_defs, i);
        explic_func(f, func_defs);
    }

    printf("Dumping internal representaiton.\n");
    for (int i = 0; i < size(func_defs); i++) {
        dump_func(*(function*)(at(func_defs, i)));
        nl;
    }
    
    printf("Done.\n");
    return EXIT_SUCCESS;
}

//pretty bad, but oh well.
void init(void) {
    func_defs = new_vector(1, sizeof(function));
    fc_div.name   = "div";
    fc_mul.name   = "mul";
    fc_plus.name  = "plus";
    fc_minus.name = "minus";

    f_div.name = malloc(sizeof(char)*3+1); sprintf(f_div.name, "div");
    f_div.params = new_vector(3, sizeof(atom));
    f_div.fully_defined = true;
    f_div.e.type = e_builtin;
    f_mul.name = malloc(sizeof(char)*3+1); sprintf(f_mul.name, "mul");
    f_mul.params = new_vector(3, sizeof(atom));
    f_mul.fully_defined = true;
    f_mul.e.type = e_builtin;
    f_plus.name = malloc(sizeof(char)*4+1); sprintf(f_plus.name, "plus");
    f_plus.params = new_vector(3, sizeof(atom));
    f_plus.fully_defined = true;
    f_plus.e.type = e_builtin;
    f_minus.name = malloc(sizeof(char)*5+1); sprintf(f_minus.name, "minus");
    f_minus.params = new_vector(3, sizeof(atom));
    f_minus.fully_defined = true;
    f_minus.e.type = e_builtin;
    atom v = { a_var, .data.vr.symbol = NULL };
    v.data.vr.symbol = malloc(sizeof(char)*2); sprintf(v.data.vr.symbol, "X");
    push_back(f_div.params, &v);
    push_back(f_mul.params, &v);
    push_back(f_plus.params, &v);
    push_back(f_minus.params, &v);
    v.data.vr.symbol = malloc(sizeof(char)*2); sprintf(v.data.vr.symbol, "Y");
    push_back(f_div.params, &v);
    push_back(f_mul.params, &v);
    push_back(f_plus.params, &v);
    push_back(f_minus.params, &v);
    v.data.vr.symbol = malloc(sizeof(char)*2); sprintf(v.data.vr.symbol, "R");
    push_back(f_div.params, &v);
    push_back(f_mul.params, &v);
    push_back(f_plus.params, &v);
    push_back(f_minus.params, &v);

    push_back(func_defs, &f_div);
    push_back(func_defs, &f_mul);
    push_back(func_defs, &f_plus);
    push_back(func_defs, &f_minus);
}

bool functor_arity_exists(vector* defs, fcall* fc) {
    for (int i = 0; i < size(defs); i++) {
        function* def = at(defs, i);
        if (strcmp(fc->name, def->name) == 0
            &&
            size(def->params) == size(fc->params)
           ) {
            return true;
        }
    }
    return false;
}

void print_help(void) {
    printf("MAFIALog compiler, very early experiment.\n");
    printf("  ./mlogc -f file.mf   : compile file \"file.mf\"\n");
    printf("  ./mlogc file.mf      :     \"           \"\n");
    printf("  ./mlogc              : interactive mode\n");
}

//all these debugging fuctions feature tons of string leaks.
void dump_func(function f) {
    //print function head
    printf("%s(", f.name);
    int ps = size(f.params);
    for (int i = 0; i < ps; i++) {
        printf("%s", atom_to_string(*(atom*)at(f.params, i)));
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

void dump_expr(expr e, bool indent) {
    char* in = indent ? "  " : "";
    switch (e.type) {
        case e_fcall : {
            //printf("%s%s", in, expr_to_string(e));
            printf("%s", in);
            dump_func_call(e.e.f);
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
        case e_and   : {
            expr e1 = *e.e.n.lhs;
            expr e2 = *e.e.n.rhs;
            dump_expr(e1, indent);
            printf(",\n");
            dump_expr(e2, indent);
        } break;
        case e_tuple : {
            dump_tuple(&e);
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

void dump_tuple(expr* e) {
    printf("(");
    rec_dump_tuple(&e->e.t.n);
    printf(")");
}

void rec_dump_tuple(and* a) {
    dump_expr(*a->lhs, false);
    printf(",");
    if (is_and_e(a->rhs)) {
        rec_dump_tuple(&a->rhs->e.n);
    } else {
        dump_expr(*a->rhs, false);
    }
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
                    sprintf(res, "%sex_$old$", res);
                }
                
                if (i != size(f.params)-1) sprintf(res, "%s, ", res);
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
    for (int i = 0; i < size(vec); i++) {
        expr* ex = at(vec, i);
        dump_expr(*ex, false); nl;
    }
}
