
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "internal_struct.h"
#include "explication.h"
#include "unify.h"
#include "generic_vector.h"
#include "generic_map.h"
#include "utils.h"
#include "func.tab.h"
#include "glob.tab.h"
#include "debug_stuff.h"

#define alot(of) 0xBadC0de

int extern errno;
extern FILE* funcin;
extern FILE* globin;

extern int func_line_number;
extern int glob_line_number;

int flag;
int verbose = 0;

vector* func_defs;
vector* func_defs_cp;
vector* global_defs;
map* global_symbol_table;

//builtins
fcall fc_div;
fcall fc_mul;
fcall fc_plus;
fcall fc_minus;
fcall fc_print;
fcall fc_nl;
fcall fc_integer;
fcall fc_lt;
fcall fc_gt;
fcall fc_cons;
fcall fc_ref;
fcall fc_at_index;
fcall fc_at_set;
fcall fc_card;
function f_div;
function f_mul;
function f_plus;
function f_minus;
function f_print;
function f_nl;
function f_less_than;
function f_cons;
function f_ref;
function f_at_index;
function f_at_set;
function f_card;
//slight hack for calling main
fcall fc_main;

void init(void);

void print_help(void);

int main(int argc, char** argv) {
    char* filename_path = NULL;
    int option;
    bool verbose_return = false;
    while ((option = getopt(argc, argv, "hvrf:")) != -1) {
        switch (option) {
            case 'h': print_help(); return EXIT_SUCCESS;       break;
            case 'v': verbose++;                               break;
            case 'f': filename_path = optarg;                  break;
            case 'r': verbose_return = true;                   break;
            case '?': printf("Unknown option : %c\n", optopt); break;
            default :                                          break;
        }
    }
    if (verbose > 0) printf("Starting.\n");
    
    vector* files = new_vector(1, sizeof(char*));
    while (optind < argc) {
        //filename_path = argv[argc-1];
        vec_push_back(files, &argv[optind]);
        optind++;
    }
    
    if (verbose > 0) printf("Initialising.\n");
    init();
    if (vec_size(files) > 0) {
        for (int i = 0; i < vec_size(files); i++) {
            char** filename = vec_at(files, i);
            FILE* file = fopen(*filename, "r");
            if (file == NULL) {
                printf("Error, could not open file %s:\n  %s\n", *filename, strerror(errno));
                return EXIT_FAILURE;
            }
            if (verbose > 0) printf("Parsing/Lexing: %s\n", *filename);
            globin = file;
            glob_line_number = 1;
            globparse();
            rewind(file);

            funcin = file;
            func_line_number = 1;
            funcparse();
            
            fclose(file);
        }
        
    } else {
        printf("Error :: No input files\n");
        exit(EXIT_FAILURE);
    }

    //decomposing equs in globals
    //pretty bad, needs rewrite
    function f;
    expr gands;
    gands.type = e_and;
    gands.e.n.ands = global_defs;
    f.e = gands;
    decompose_equs(&f);

    if (verbose > 1) {
        printf("Post parse, pre-preproc:\n");
        for (int i = 0; i < vec_size(func_defs); i++) {
            function* f = vec_at(func_defs, i);
            dump_func(*f); nl;
        }
    }

    if (verbose > 0) printf("Explicating constants/variables.\n");
    for (int i = 0; i < vec_size(func_defs); i++) {
        function* f = vec_at(func_defs, i);
        explic_func(f, func_defs);
    }
    free_vector(func_defs);//double check this

    if (verbose > 0) {
        printf("Dumping choice points.\n");
        for (int i = 0; i < vec_size(func_defs_cp); i++) {
            dump_cp(vec_at(func_defs_cp, i));
        }
        printf("Dumping globals.\n");
        for (int i = 0; i < vec_size(global_defs); i++) {
            expr* ex = vec_at(global_defs, i);
            dump_expr(*ex, true); nl;
        }
        printf("\nRunning entry.\n");
    }
    
    entry(func_defs_cp, global_defs, verbose_return);
    
    //do cleanup
    if (verbose > 0) printf("Done.\n");
    return EXIT_SUCCESS;
}

//pretty bad, but oh well.
void init(void) {
    func_defs = new_vector(4, sizeof(function));
    func_defs_cp = new_vector(4, sizeof(choice_point));
    global_defs = new_vector(0, sizeof(expr));
    global_symbol_table = new_map(sizeof(char*), sizeof(int));
    set_fst_comparator(global_symbol_table, string_compare);
    set_snd_comparator(global_symbol_table, byte_compare);

    fc_div.name      = strdup("div");
    fc_mul.name      = strdup("mul");
    fc_plus.name     = strdup("plus");
    fc_minus.name    = strdup("minus");
    fc_print.name    = strdup("print");
    fc_nl.name       = strdup("nl");
    fc_integer.name  = strdup("integer");
    fc_lt.name       = strdup("less_than");
    fc_gt.name       = strdup("greater_than");
    fc_cons.name     = strdup("cons");
    fc_ref.name      = strdup("ref");
    fc_at_index.name = strdup("at");
    fc_card.name     = strdup("card");
    fc_div.res_set   = fc_mul.res_set  = fc_plus.res_set = rs_one;
    fc_minus.res_set = fc_plus.res_set = fc_cons.res_set = rs_one;
    fc_lt.res_set    = fc_gt.res_set   = fc_ref.res_set  = rs_one;
    fc_at_index.res_set = fc_at_set.res_set = fc_card.res_set = rs_one;

    fc_minus.type = f_user;
    fc_print.type = fc_nl.type  = fc_integer.type = fc_lt.type = f_builtin;
    fc_cons.type  = fc_ref.type = fc_at_index.type = fc_at_set.type = f_builtin;
    fc_card.type  = f_builtin;

    fc_main.name = strdup("main");
    fc_main.params = new_vector(1, sizeof(expr));
    fc_main.type = f_user;

    ///TODO: remove aaaaall of this
    f_div.name   = strdup("div");
    f_mul.name   = strdup("mul");
    f_plus.name  = strdup("plus");
    f_minus.name = strdup("minus");
    f_print.name = strdup("print");
    f_nl.name    = strdup("nl");
    f_less_than.name = strdup("less_than");
    f_cons.name  = strdup("cons");
    f_ref.name   = strdup("ref");
    f_at_index.name = strdup("at_index");
    f_at_set.name   = strdup("at_set");
    f_card.name  = strdup("card");
    f_div.params   = new_vector(3, sizeof(atom));
    f_mul.params   = new_vector(3, sizeof(atom));
    f_plus.params  = new_vector(3, sizeof(atom));
    f_minus.params = new_vector(3, sizeof(atom));
    f_print.params = new_vector(1, sizeof(atom));
    f_nl.params    = new_vector(1, sizeof(atom));
    f_less_than.params = new_vector(2, sizeof(atom));
    f_cons.params  = new_vector(0, 1);
    f_ref.params   = new_vector(2, sizeof(atom));
    f_at_index.params = new_vector(3, sizeof(atom));
    f_at_set.params   = new_vector(3, sizeof(atom));
    f_card.params  = new_vector(2, sizeof(atom));
    f_div.fully_defined  = f_mul.fully_defined   =
    f_plus.fully_defined = f_minus.fully_defined = true;//needs work
    f_div.e.type   = f_mul.e.type       =
    f_plus.e.type  = f_minus.e.type     = 
    f_print.e.type = f_less_than.e.type =
    f_nl.e.type    = f_cons.e.type      =
    f_ref.e.type   = f_at_index.e.type  =
    f_at_set.e.type = f_card.e.type = e_builtin;
    f_div.type   = f_mul.type       =
    f_plus.type  = f_minus.type     =
    f_print.type = f_less_than.type = 
    f_nl.type    = f_cons.type      =
    f_ref.type   = f_at_index.type  =
    f_at_set.type = f_card.type     = fd_func;

    atom v = {.type = a_var, .data.vr = {.symbol = {.type = s_var, .scope = -1, .num = 0}}};
    vec_push_back(f_div.params, &v);
    vec_push_back(f_mul.params, &v);
    vec_push_back(f_plus.params, &v);
    vec_push_back(f_minus.params, &v);
    vec_push_back(f_print.params, &v);
    vec_push_back(f_nl.params, &v);
    vec_push_back(f_less_than.params, &v);
    vec_push_back(f_ref.params, &v);
    vec_push_back(f_at_index.params, &v);
    vec_push_back(f_at_set.params, &v);
    vec_push_back(f_card.params, &v);
    v.data.vr.symbol.num++;
    vec_push_back(f_div.params, &v);
    vec_push_back(f_mul.params, &v);
    vec_push_back(f_plus.params, &v);
    vec_push_back(f_minus.params, &v);
    vec_push_back(f_less_than.params, &v);
    vec_push_back(f_ref.params, &v);
    vec_push_back(f_at_index.params, &v);
    vec_push_back(f_at_set.params, &v);
    vec_push_back(f_card.params, &v);
    v.data.vr.symbol.num++;
    vec_push_back(f_div.params, &v);
    vec_push_back(f_mul.params, &v);
    vec_push_back(f_plus.params, &v);
    vec_push_back(f_minus.params, &v);
    vec_push_back(f_at_index.params, &v);
    vec_push_back(f_at_set.params, &v);

    //NOTE
    expr ev = {.type = e_atom, .e.a = v};
    vec_push_back(fc_main.params, &ev);

    vec_push_back(func_defs, &f_div);
    vec_push_back(func_defs, &f_mul);
    vec_push_back(func_defs, &f_plus);
    vec_push_back(func_defs, &f_minus);
    vec_push_back(func_defs, &f_print);
    vec_push_back(func_defs, &f_nl);
    vec_push_back(func_defs, &f_less_than);
    vec_push_back(func_defs, &f_cons);
    vec_push_back(func_defs, &f_ref);
    vec_push_back(func_defs, &f_at_index);
    vec_push_back(func_defs, &f_at_set);
    vec_push_back(func_defs, &f_card);

    choice_point c_div;
    c_div.functions = new_vector(1, sizeof(function));
    vec_push_back(c_div.functions, &f_div);
    vec_push_back(func_defs_cp, &c_div);
    
    choice_point c_mul;
    c_mul.functions = new_vector(1, sizeof(function));
    vec_push_back(c_mul.functions, &f_mul);
    vec_push_back(func_defs_cp, &c_mul);
    
    choice_point c_plus;
    c_plus.functions = new_vector(1, sizeof(function));
    vec_push_back(c_plus.functions, &f_plus);
    vec_push_back(func_defs_cp, &c_plus);
    
    //minus is now found in std.mf
    //choice_point c_minus;
    //c_minus.functions = new_vector(1, sizeof(function));
    //vec_push_back(c_minus.functions, &f_minus);
    //vec_push_back(func_defs_cp, &c_minus);

    choice_point c_print;
    c_print.functions = new_vector(1, sizeof(function));
    vec_push_back(c_print.functions, &f_print);
    vec_push_back(func_defs_cp, &c_print);

    choice_point c_nl;
    c_nl.functions = new_vector(1, sizeof(function));
    vec_push_back(c_nl.functions, &f_nl);
    vec_push_back(func_defs_cp, &c_nl);

    choice_point c_cons;
    c_cons.functions = new_vector(1, sizeof(function));
    vec_push_back(c_cons.functions, &f_cons);
    vec_push_back(func_defs_cp, &c_cons);

    choice_point c_ref;
    c_ref.functions = new_vector(1, sizeof(function));
    vec_push_back(c_ref.functions, &f_ref);
    vec_push_back(func_defs_cp, &c_ref);

    choice_point c_at_index;
    c_at_index.functions = new_vector(1, sizeof(function));
    vec_push_back(c_at_index.functions, &f_at_index);
    vec_push_back(func_defs_cp, &c_at_index);

    choice_point c_at_set;
    c_at_set.functions = new_vector(1, sizeof(function));
    vec_push_back(c_at_set.functions, &f_at_set);
    vec_push_back(func_defs_cp, &c_at_set);

    choice_point c_card;
    c_card.functions = new_vector(1, sizeof(function));
    vec_push_back(c_card.functions, &f_card);
    vec_push_back(func_defs_cp, &c_card);
}

void print_help(void) {
    printf("MAFIALog compiler, very early experiment.\n");
    printf("  ./mlogc -f file.mf   : compile file \"file.mf\"\n");
    printf("  ./mlogc file.mf      :     \"           \"\n");
    printf("  -v[v]                : verbose/debug    \n");
    printf("  -r                   : show mains returned result and answers\n");
    printf("  ./mlogc              : interactive mode\n");
}
