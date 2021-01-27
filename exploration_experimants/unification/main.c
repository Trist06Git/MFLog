
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "internal_struct.h"
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

void disimp_func(function*);
void disimp_func_expr(function*);
void disimp_func_head(function*);
void rec_disimp_func_expr(expr*, int* unique, vector* fcs_to_move, vector* singletons);
void rec_disimp_func_equ(expr*, int* unique, vector* fc_to_move, vector* singles);
void disimp_func_params(fcall*, int* unique, vector* fcs_to_move, vector* singletons);
void disimp_callsite(fcall*, int* unique, vector* newly_generated);
//I think disimplicate actually means to nolonger imply any connection at all
//explicate should probably be used instead

vector* get_var_singles(function*);
void rec_get_var_singles(expr*, map*);
void check_var_singles_params(vector*, map*);

function* get_fdef(vector*, char* name);
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
        disimp_func(f);
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

void disimp_func(function* f) {
    if (f->fully_defined) return;
    disimp_func_expr(f);
    disimp_func_head(f);
    f->fully_defined = true;
}

void disimp_func_expr(function* f) {
    int unique = 0;
    vector* moved_fcalls = new_vector(10, sizeof(expr));
    vector* singletons = get_var_singles(f);
    rec_disimp_func_expr(&f->e, &unique, moved_fcalls, singletons);
    //and append moved fcalls
    for (int i = 0; i < size(moved_fcalls); i++) {
        expr* fc = at(moved_fcalls, i);
        append_expr(&f->e, fc);
    }
}

void rec_disimp_func_expr(expr* e, int* unique, vector* fc_to_move, vector* singletons) {
    if (is_val_e(e)) {
        //replace with equality and unique var
        expr new_left  = make_var_e(unique_name_incr(unique));
        expr new_right = *e;
        e->type = e_equ;
        e->e.e.lhs = malloc(sizeof(expr));
        e->e.e.rhs = malloc(sizeof(expr));
        *e->e.e.lhs = new_left;
        *e->e.e.rhs = new_right;
    } else if (is_and_e(e)) {
        rec_disimp_func_expr(e->e.n.lhs, unique, fc_to_move, singletons);
        rec_disimp_func_expr(e->e.n.rhs, unique, fc_to_move, singletons);
    } else if (is_fcall_e(e)) {
        function* def = get_fdef(func_defs, e->e.f.name);
        if (def == NULL) {
            printf("Error, unknown function %s with arity %i.", e->e.f.name, size(e->e.f.params));
            return;
        }
        disimp_func(def);
        disimp_func_params(&e->e.f, unique, fc_to_move, singletons);
        disimp_callsite(&e->e.f, unique, NULL);
    } else if (is_equ_e(e)) {
        //printf("dis-imp of fcalls in equations not done yet..\n");
        rec_disimp_func_equ(e, unique, fc_to_move, singletons);
    }
}

void rec_disimp_func_equ(expr* e, int* unique, vector* fc_to_move, vector* singles) {
    if (is_equ_e(e)) {
        rec_disimp_func_equ(e->e.n.lhs, unique, fc_to_move, singles);
        rec_disimp_func_equ(e->e.n.rhs, unique, fc_to_move, singles);
    } else if (is_fcall_e(e)) {
        rec_disimp_func_expr(e, unique, fc_to_move, singles);
        //move fcall out
        //for now this only substitues the last parameter of the fcall
        //it should be changed to substitute all unbound variables anded
        //togeather, resembling a tuple
        push_back(fc_to_move, e);
        expr* last_var = at(e->e.f.params, size(e->e.f.params)-1);
        *e = copy_var_e(last_var);
    }
}

void disimp_func_params(fcall* fc, int* unique, vector* fc_to_move, vector* singletons) {
    for (int i = 0; i < size(fc->params); i++) {
        expr* e = at(fc->params, i);
        expr param = *e;
        if (is_fcall_e(&param)) {
            function* def = get_fdef(func_defs, param.e.f.name);
            if (def == NULL) {
                printf("Error, unknown function %s with arity %i.", e->e.f.name, size(e->e.f.params));
                return;
            }
            disimp_func(def);
            disimp_func_params(&param.e.f, unique, fc_to_move, singletons);
            vector* newly_generated = new_vector(0, sizeof(expr));
            disimp_callsite(&param.e.f, unique, newly_generated);
            ////decompose func
            //push_back newly generated
            for (int j = 0; j < size(newly_generated); j++) {
                insert_at(fc->params, i, at(newly_generated, j)); i++;
            }
            free_vector(newly_generated);
            //push_back singletons
            for (int j = 0; j < size(param.e.f.params); j++) {
                expr* pparam = at(param.e.f.params, j);
                for (int k = 0; k < size(singletons); k++) {
                    char* sing = at(singletons, k);
                    if (is_var_e(pparam)) {
                        if (strcmp(sing, pparam->e.a.data.vr.symbol) == 0) {
                            //is a singleton
                            char* temp = malloc(sizeof(char)*(strlen(sing)+1));
                            expr new_var = make_var_e(temp);
                            insert_at(fc->params, i, &new_var); i++;
                        }
                    }   
                }
            }
            push_back(fc_to_move, &param);
            remove_at(fc->params, i); i--;
        }
    }
}

//newly_generated is optional, if not null, it will contain the newly
//generated variables
void disimp_callsite(fcall* fc, int* unique, vector* newly_generated) {
    function* def = get_fdef(func_defs, fc->name);
    if (def == NULL) {
        printf("Error, unknown function %s with arity %i.", fc->name, size(fc->params));
        return;
    }
    while (size(fc->params) < size(def->params)) {
        expr new_name = make_var_e(unique_name_incr(unique));
        push_back(fc->params, &new_name);
        if (newly_generated != NULL) {
            push_back(newly_generated, &new_name);
        }
    }
}

void disimp_func_head(function* f) {
    vector* f_singles = get_var_singles(f);
    for (int i = 0; i < size(f_singles); i++) {
        char* s = at(f_singles, i);
        char* s_new_param = malloc(sizeof(char)*strlen(s)+1);
        strcpy(s_new_param, s);
        atom a_new_param = {a_var, .data.vr.symbol = s_new_param};
        push_back(f->params, &a_new_param);
    }

    free_vector(f_singles);
}

vector* get_var_singles(function* f) {
    //varname, count
    map* mp = new_map(sizeof(char*), sizeof(int));
    mp->el1_comparator = string_compare;
    rec_get_var_singles(&f->e, mp);
    check_var_singles_params(f->params, mp);
    vector* singletons = new_vector(0, sizeof(char*));
    for (int i = 0; i < map_size(mp); i++) {
        pair* kv = map_at_index(mp, i);
        if (*(int*)kv->snd == 1) {
            push_back(singletons, *(char**)kv->fst);//check this.
        }
    }
    free_map(mp);
    return singletons;
}

void rec_get_var_singles(expr* ex, map* mp) {
    if (is_var_e(ex)) {
        char* v_name = ex->e.a.data.vr.symbol;
        if (map_contains_key(mp, &v_name)) {
            (*(int*)snd(mp, &v_name))++;
        } else {
            int one = 1;
            map_add(mp, &v_name, &one);
        }
    } else if (is_and_e(ex)) {
        rec_get_var_singles(ex->e.n.lhs, mp);
        rec_get_var_singles(ex->e.n.rhs, mp);
    } else if (is_equ_e(ex)) {
        rec_get_var_singles(ex->e.e.lhs, mp);
        rec_get_var_singles(ex->e.e.rhs, mp);
    } else if (is_fcall_e(ex)) {
        vector* ps = ex->e.f.params;
        for (int i = 0; i < size(ps); i++) {
            expr* psi = at(ps, i);
            rec_get_var_singles(psi, mp);
        }
    } else if (is_tuple_e(ex)) {
        expr temp;
        temp.type = e_and;
        temp.e.n = ex->e.t.n;
        rec_get_var_singles(&temp, mp);
    }
}

void check_var_singles_params(vector* params, map* mp) {
    for (int i = 0; i < size(params); i++) {
        atom* pr = at(params, i);
        if (is_var_a(pr)) {
            char* v_name = pr->data.vr.symbol;
            if (map_contains_key(mp, &v_name)) {
                (*(int*)snd(mp, &v_name))++;
            }
        }
    }
}

function* get_fdef(vector* defs, char* name) {
    for (int i = 0; i < size(defs); i++) {
        function* f = at(defs, i);
        if (strcmp(name, f->name) == 0) {
            return f;
        }
    }
    return NULL;
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

void dump_expr_vec(vector* vec) {
    for (int i = 0; i < size(vec); i++) {
        expr* ex = at(vec, i);
        dump_expr(*ex, false); nl;
    }
}
