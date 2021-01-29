
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
#include "y.tab.h"
#include "debug_stuff.h"

int extern errno;
extern FILE* yyin;

int flag;
int verbose = 0;

vector* func_defs;
vector* func_defs_cp;

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
    free_vector(func_defs);//double check this

    /*printf("Dumping internal representaiton.\n");
    for (int i = 0; i < size(func_defs); i++) {
        dump_func(*(function*)(at(func_defs, i)));
        nl;
    }*/

    printf("Dumping choice points.\n");
    for (int i = 0; i < size(func_defs_cp); i++) {
        dump_cp(at(func_defs_cp, i));
    }

    printf("Running entry.\n");
    entry(func_defs_cp);
    
    printf("Done.\n");
    return EXIT_SUCCESS;
}

//pretty bad, but oh well.
void init(void) {
    func_defs = new_vector(4, sizeof(function));
    func_defs_cp = new_vector(4, sizeof(choice_point));
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

    choice_point c_div;
    c_div.functions = new_vector(1, sizeof(function));
    push_back(c_div.functions, &f_div);
    push_back(func_defs_cp, &c_div);
    
    choice_point c_mul;
    c_mul.functions = new_vector(1, sizeof(function));
    push_back(c_mul.functions, &f_mul);
    push_back(func_defs_cp, &c_mul);
    
    choice_point c_plus;
    c_plus.functions = new_vector(1, sizeof(function));
    push_back(c_plus.functions, &f_plus);
    push_back(func_defs_cp, &c_plus);
    
    choice_point c_minus;
    c_minus.functions = new_vector(1, sizeof(function));
    push_back(c_minus.functions, &f_minus);
    push_back(func_defs_cp, &c_minus);
}

void print_help(void) {
    printf("MAFIALog compiler, very early experiment.\n");
    printf("  ./mlogc -f file.mf   : compile file \"file.mf\"\n");
    printf("  ./mlogc file.mf      :     \"           \"\n");
    printf("  ./mlogc              : interactive mode\n");
}


