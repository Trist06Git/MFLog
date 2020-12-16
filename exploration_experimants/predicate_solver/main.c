
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "y.tab.h"
#include "generic_vector.h"
#include "predicate.h"

extern FILE* yyin;

int flag;
vector* names;
vector* preds;
vector* calls;
predicate taut = {.name = "true"};
predicate cont = {.name = "false"};

bool eval_call(predicate*);
vector* get_pred(char* name);
void print_help(void);
void dump_predicates(void);

int verbose = 0;

int main(int argc, char** argv) {
    char* filename = NULL;
    int option;
    while ((option = getopt(argc, argv, "hvf:")) != -1) {
        switch (option) {
            case 'h': print_help(); return EXIT_SUCCESS;       break;
            case 'v': verbose++;                               break;
            case 'f': filename = optarg;                       break;
            case '?': printf("Unknown option : %c\n", optopt); break;
            default :                                          break;
        }
    }
    if (verbose > 0) printf("Starting.\n");
    if (optind < argc) filename = argv[argc-1];
    if (filename != NULL) {
        if (verbose > 0) printf("Loading file : %s\n", filename);\
        FILE* file = fopen(filename, "r");
        if (file == NULL) {
            printf("Error, could not open file\n");
            return EXIT_FAILURE;
        }
        yyin = file;
    } else if (verbose > 0) {
        printf("Entering interactive mode\n");
    }

    names = new_vector(0, sizeof(char*));
    preds = new_vector(0, sizeof(predicate));
    calls = new_vector(0, sizeof(predicate));
    yyparse();
    if (flag == 1) {
        printf("\nInvalid expression!\n");
        return EXIT_FAILURE;
    }

    if (verbose > 1) dump_predicates();
    for (int i = 0; i < size(calls); i++) {
        if (verbose > 1) printf("Stack trace :\n");
        bool res = eval_call(at(calls, i));
        printf(":- %s\n", res ? "true" : "false");
    }
    
    free_vector(names);
    free_vector(preds);
    free_vector(calls);

    if (verbose > 0) printf("Done.\n");
    return EXIT_SUCCESS;
}

bool eval_call(predicate* p) {
    if (verbose > 1) printf("-> %s\n", p->name);
    if (strcmp(p->name, "false") == 0) {
        return false;
    } else if (strcmp(p->name, "true") == 0) {
        return true;
    } else {
        for (int i = 0; i < size(p->subs); i++) {
            vector* sub = get_pred((char*)at(p->subs, i));
            if (sub == NULL || size(sub) == 0) return false;
            bool retry = false;
            for (int j = 0; j < size(sub); j++) {
                predicate* s = at(sub, j);
                retry |= eval_call(s);
            }
            if (retry == false) return false;
        }
    }
    return true;
}

//not very good..
vector* get_pred(char* name) {
    vector* res = new_vector(0, sizeof(predicate));
    if (strcmp(name, "false") == 0) {
        push_back(res, &cont);
    } else if (strcmp(name, "true") == 0) {
        push_back(res, &taut);
    } else {
        for (int i = 0; i < size(preds); i++) {
            predicate* p = at(preds, i);
            if (strcmp(name, p->name) == 0) {
                push_back(res, p);//copying in..
            }
        }
        if (size(res) == 0) push_back(res, &cont);
    }
    return res;
}

void print_help(void) {
    char* red = "\033[0;31m";
    char* clear = "\033[0m";
    printf("First Order Logical %sP%sredicate %sC%sompililer (%sPC%s)\n", red, clear, red, clear, red, clear);
    printf("Usage    : \"pc <filename>\" or \"pc -f <filename>\"\n");
    printf("Options  : -v for verbose\n");
    printf("         : -h for this message\n");
    printf("Language : The input language is similar to Prolog, except\n");
    printf("         : that there are no variables or unification.\n");
}

void dump_predicates(void) {
    printf("Database :\nPredicates :\n");
    for (int i = 0; i < size(preds); i++) {
        predicate* p = at(preds, i);
        printf("%s :-\n    ", p->name);
        for (int j = 0; j < size(p->subs); j++) {
            printf("%s, ", (char*)at(p->subs, j));
        }
        printf("\n");
    }
    printf("calls :\n");
    for (int i = 0; i < size(calls); i++) {
        predicate* c = at(calls, i);
        printf("  ");
        for (int j = 0; j < size(c->subs); j++) {
            printf("%s, ", (char*)at(c->subs, j));
        }
        printf("\n");
    }
    printf("\n");
}
