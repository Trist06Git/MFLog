
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

int extern errno;
extern FILE* funcin;
extern FILE* globin;

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
function f_div;
function f_mul;
function f_plus;
function f_minus;
function f_print;
function f_nl;
function f_less_than;
function f_cons;
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
            globparse();
            rewind(file);

            funcin = file;
            funcparse();
            
            fclose(file);
        }
        
    } else if (verbose > 0) {
        printf("Entering inspection mode\n");
    }

    /*if (filename_path != NULL) {
        if (verbose > 0) printf("Loading file : %s\n", filename_path);
        FILE* file = fopen(filename_path, "r");
        if (file == NULL) {
            printf("Error, could not open file %s:\n  %s\n", filename_path, strerror(errno));
            return EXIT_FAILURE;
        }
        yyin = file;
        //WARN. file not closed yet.
    } else if (verbose > 0) {
        printf("Entering inspection mode\n");
    }*/

    //decomposing equs in globals
    //pretty bad, needs rewrite
    function f;
    expr gands;
    gands.type = e_and;
    gands.e.n.ands = global_defs;
    f.e = gands;
    decompose_equs(&f);

    //printf("Post parse, pre-preproc:\n");
    //for (int i = 0; i < vec_size(func_defs); i++) {
    //    function* f = vec_at(func_defs, i);
    //    dump_func(*f); nl;
    //}

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

    fc_div.name     = "div";
    fc_mul.name     = "mul";
    fc_plus.name    = "plus";
    fc_minus.name   = "minus";
    fc_print.name   = "print";
    fc_nl.name      = "nl";
    fc_integer.name = "integer";
    fc_lt.name      = "less_than";
    fc_gt.name      = "greater_than";
    fc_cons.name    = "cons";
    fc_div.res_set   = fc_mul.res_set  = fc_plus.res_set =
    fc_minus.res_set = fc_plus.res_set = fc_cons.res_set = rs_one;
    fc_lt.res_set    = fc_gt.res_set   = rs_one;

    fc_print.type = fc_nl.type = fc_integer.type = fc_lt.type =
    fc_cons.type  = f_builtin;

    fc_main.name = "main";
    fc_main.params = new_vector(1, sizeof(expr));
    fc_main.type = f_user;

    ///TODO: remove aaaaall of this
    f_div.name   = malloc(sizeof(char)*3+1); sprintf(f_div.name, "div");
    f_mul.name   = malloc(sizeof(char)*3+1); sprintf(f_mul.name, "mul");
    f_plus.name  = malloc(sizeof(char)*4+1); sprintf(f_plus.name, "plus");
    f_minus.name = malloc(sizeof(char)*5+1); sprintf(f_minus.name, "minus");
    f_print.name = malloc(sizeof(char)*5+1); sprintf(f_print.name, "print");
    f_nl.name    = malloc(sizeof(char)*2+1); sprintf(f_nl.name, "nl");
    f_less_than.name = malloc(sizeof(char)*9+1); sprintf(f_less_than.name, "less_than");
    f_cons.name  = malloc(sizeof(char)*4+1); sprintf(f_cons.name, "cons");
    f_div.params   = new_vector(3, sizeof(atom));
    f_mul.params   = new_vector(3, sizeof(atom));
    f_plus.params  = new_vector(3, sizeof(atom));
    f_minus.params = new_vector(3, sizeof(atom));
    f_print.params = new_vector(1, sizeof(atom));
    f_nl.params = new_vector(1, sizeof(atom));
    f_less_than.params = new_vector(2, sizeof(atom));
    f_cons.params = new_vector(0, 1);
    f_div.fully_defined  = f_mul.fully_defined   =
    f_plus.fully_defined = f_minus.fully_defined = true;//needs work
    f_div.e.type   = f_mul.e.type       =
    f_plus.e.type  = f_minus.e.type     = 
    f_print.e.type = f_less_than.e.type =
    f_nl.e.type    = f_cons.e.type      = e_builtin;
    f_div.type   = f_mul.type       =
    f_plus.type  = f_minus.type     =
    f_print.type = f_less_than.type = 
    f_nl.type    = f_cons.type      = fd_func;

    atom v = {.type = a_var, .data.vr = {.symbol = {.type = s_var, .scope = -1, .num = 0}}};
    //v.data.vr.symbol = malloc(sizeof(char)*2); sprintf(v.data.vr.symbol, "X");
    vec_push_back(f_div.params, &v);
    vec_push_back(f_mul.params, &v);
    vec_push_back(f_plus.params, &v);
    vec_push_back(f_minus.params, &v);
    vec_push_back(f_print.params, &v);
    vec_push_back(f_nl.params, &v);
    vec_push_back(f_less_than.params, &v);
    //v.data.vr.symbol = malloc(sizeof(char)*2); sprintf(v.data.vr.symbol, "Y");
    v.data.vr.symbol.num++;
    vec_push_back(f_div.params, &v);
    vec_push_back(f_mul.params, &v);
    vec_push_back(f_plus.params, &v);
    vec_push_back(f_minus.params, &v);
    vec_push_back(f_less_than.params, &v);
    //v.data.vr.symbol = malloc(sizeof(char)*2); sprintf(v.data.vr.symbol, "R");
    v.data.vr.symbol.num++;
    vec_push_back(f_div.params, &v);
    vec_push_back(f_mul.params, &v);
    vec_push_back(f_plus.params, &v);
    vec_push_back(f_minus.params, &v);

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
    
//    choice_point c_minus;
//    c_minus.functions = new_vector(1, sizeof(function));
//    vec_push_back(c_minus.functions, &f_minus);
//    vec_push_back(func_defs_cp, &c_minus);

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
}

void print_help(void) {
    printf("MAFIALog compiler, very early experiment.\n");
    printf("  ./mlogc -f file.mf   : compile file \"file.mf\"\n");
    printf("  ./mlogc file.mf      :     \"           \"\n");
    printf("  -v                   : verbose/debug    \n");
    printf("  -r                   : show mains returned result and answers\n");
    printf("  ./mlogc              : interactive mode\n");
}
