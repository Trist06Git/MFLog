
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
int verbose = 0;//not used yet
int unique_main = 0;//old

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

void name_vals_expr(expr* e, int* id);
void name_vals_and(and* nd, int* id);
void name_vals_func(function*);

void bind_func_returns(function*);
vector* get_var_singles(function*);
void rec_get_var_singles(expr*, map*);
void check_var_singles_params(vector*, map*);
void decompose_fcalls(function*);
void rec_decompose_fcalls(fcall*, vector* singles, vector* outer_exs, vector* ret_vars, bool root_fcall);

function* get_fdef(vector*, char* name);
bool functor_arity_exists(vector* defs, fcall*);

void dump_func(function);
void dump_expr(expr, bool);
void dump_func_call(fcall);
char* expr_to_string(expr);
char* atom_to_string(atom);

void print_help(void);

int main(int argc, char** argv) {
    char* filename_path = NULL;
    int option;
    while ((option = getopt(argc, argv, "hvf:")) != -1) {
        switch (option) {
            case 'h': print_help(); return EXIT_SUCCESS;       break;
            case 'v': verbose++;                               break;
            case 'f': filename_path = optarg;                       break;
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

    printf("Disimplicating constants/variables.\n");
    for (int i = 0; i < size(func_defs); i++) {
        function* f = at(func_defs, i);
        if (!f->fully_defined) name_vals_func(f);
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

void name_vals_expr(expr* e, int* id) {
    if (is_val_e(*e)) {
        expr old = *e;
        e->type = e_equ;
        e->e.e.lhs = malloc(sizeof(expr));
        e->e.e.rhs = malloc(sizeof(expr));
        memcpy(e->e.e.rhs, &old, sizeof(old));
        expr new_name = make_var_e(unique_name_incr(id));
        memcpy(e->e.e.lhs, &new_name, sizeof(new_name));
    } else if (is_and_e(*e)) {
        name_vals_and(&e->e.n, id);
    } else if (is_fcall_e(*e)) {
        fcall* fc = &e->e.f;
        function* def = get_fdef(func_defs, fc->name);
        if (def != NULL) {
            if (def->fully_defined) {
                printf("%s is already fully defined, skipping\n", def->name);
            } else {
                name_vals_func(def);
            }
            //disimplicate fcalls params
            for (int i = 0; i < size(fc->params); i++) {
                expr* par = at(fc->params, i);
                if (is_fcall_e(*par)) {
                    name_vals_expr(par, id);
                }
            }
            //disimplicate the call sites variables
            while (size(fc->params) < size(def->params)) {
                expr new_name = make_var_e(unique_name_incr(id));
                push_back(fc->params, &new_name);
            }
        } else {
            printf("Error. Unknown function %s, with arity %i\n", fc->name, size(fc->params));
        }
    }
}

//skipping the equ's so only hitting the unbound values
void name_vals_and(and* nd, int* id) {
    name_vals_expr(nd->lhs, id);//lhs is always expr
    name_vals_expr(nd->rhs, id);
}

//need to do func decomposition as a second pass
void name_vals_func(function* f) {
    //printf("Naming vals for %s.\n", f->name);
    int unique = 0;
    name_vals_expr(&f->e, &unique);
    decompose_fcalls(f);
    bind_func_returns(f);
    f->fully_defined = true;
}

void bind_func_returns(function* f) {
    vector* f_singles = get_var_singles(f);
    //printf("%s singleton varibles :\n", f->name);
    for (int i = 0; i < size(f_singles); i++) {
        char* s = at(f_singles, i);
        //printf("%s\n", s);
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
    //free_map(mp)??
    return singletons;
}

void rec_get_var_singles(expr* ex, map* mp) {
    if (is_var_e(*ex)) {
        char* v_name = ex->e.a.data.vr.symbol;
        if (map_contains_key(mp, &v_name)) {
            (*(int*)snd(mp, &v_name))++;
        } else {
            int one = 1;
            map_add(mp, &v_name, &one);
        }
    } else if (is_and_e(*ex)) {
        rec_get_var_singles(ex->e.n.lhs, mp);
        rec_get_var_singles(ex->e.n.rhs, mp);
    } else if (is_equ_e(*ex)) {
        rec_get_var_singles(ex->e.e.lhs, mp);
        rec_get_var_singles(ex->e.e.rhs, mp);
    } else if (is_fcall_e(*ex)) {
        vector* ps = ex->e.f.params;
        for (int i = 0; i < size(ps); i++) {
            expr* psi = at(ps, i);
            rec_get_var_singles(psi, mp);
        }
    }
}

void check_var_singles_params(vector* params, map* mp) {
    for (int i = 0; i < size(params); i++) {
        atom* pr = at(params, i);
        if (is_var_a(*pr)) {
            char* v_name = pr->data.vr.symbol;
            if (map_contains_key(mp, &v_name)) {
                (*(int*)snd(mp, &v_name))++;
            }
        }
    }
}

//this is horrific and needs to be redone...
void decompose_fcalls(function* f) {
    vector* singles = get_var_singles(f);
    expr* ex = &f->e;
    vector* moved_fcalls = new_vector(0, sizeof(expr));
    vector* ret_vars = new_vector(0, sizeof(expr));
    
    if (is_fcall_e(*ex)) {//singlular fcall in f's expression
        rec_decompose_fcalls(&ex->e.f, singles, moved_fcalls, ret_vars, true);
    } else {
        while (is_and_e(*ex)) {
            if (is_fcall_e(*ex->e.n.lhs))
                rec_decompose_fcalls(&ex->e.n.lhs->e.f, singles, moved_fcalls, ret_vars, true);
            ex = ex->e.n.rhs;
        }
        //final and
        if (is_fcall_e(*ex))
            rec_decompose_fcalls(&ex->e.f, singles, moved_fcalls, ret_vars, true);
    }
    //printf("These function calls were composed and will be moved:\n");
    for (int i = 0; i < size(moved_fcalls); i++) {
        expr* efc = at(moved_fcalls, i);
        append_expr(ex, efc);
    }
    free_vector(moved_fcalls);
    free_vector(ret_vars);
    free_vector(singles);
}

void rec_decompose_fcalls(fcall* fc, vector* singles, vector* outer_exs, vector* ret_vars, bool root_fcall) {
    for (int i = 0; i < size(fc->params); i++) {
        expr* par = at(fc->params, i);
        //printf("checking param %i/%i of func %s..\n", i, size(fc->params)-1, fc->name);
        if (is_fcall_e(*par)) {
            rec_decompose_fcalls(&par->e.f, singles, outer_exs, ret_vars, false);
            //move to outer
            push_back(outer_exs, par);
            for (int j = 0; j < size(ret_vars); j++) {//add the return vars in fcalls place
                expr* ret = at(ret_vars, j);
                //printf("inserting %s to %s's params\n", ret->e.a.data.vr.symbol, fc->name);
                insert_at(fc->params, i, ret);
                i++;
            }
            ret_vars->count = 0;//WARN! safe hack for vector.clear
            remove_at(fc->params, i); i--;
        } else if (is_var_e(*par)/* && !root_fcall*/) {
            //if is single, insert to params, move fcall the outer_ex
            for (int j = 0; j < size(singles); j++) {
                char* sing = at(singles, j);
                if (strcmp(par->e.a.data.vr.symbol, sing) == 0) {
                    //printf("found matching singleton in %s: %s\n", fc->name, sing);
                    char* temp = malloc(strlen(sing)*sizeof(char));
                    strcpy(temp, sing);
                    expr new_par = make_var_e(temp);
                    push_back(ret_vars, &new_par);
                    //printf("need to insert to params: %s\n", new_par.e.a.data.vr.symbol);
                }
            }
        } else {
            //printf("fcall %s has a nonvar/nonfcall param\n", fc->name);
        }
    }
    //a bit of a hack.., needs redoing
    if (!functor_arity_exists(func_defs, fc)) {
        printf("Error, function %s with arity %i does not exist.\n", fc->name, size(fc->params));
        //exit(EXIT_FAILURE);
    }
}

function* get_fdef(vector* defs, char* name) {
    for (int i = 0; i < size(defs); i++) {
        function* f = at(defs, i);
        if (strcmp(name, f->name) == 0) {
            return f;
        }
    }
    return NULL;//WARN
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
            unique_main = 0;
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
                    sprintf(res, "%sex_%i", res, unique_main++);
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
