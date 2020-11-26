
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "runtime.h"
#include "lex.h"
#include "parse.h"
#include "build.h"

extern vector* database;

void print_help(void);
char* load_file(const char* filename);

int main(int argc, char** argv) {
    bool verbose = false;
    char* filename = NULL;
    int option;
    while ((option = getopt(argc, argv, "hvf:")) != -1) {
        switch (option) {
            case 'h': print_help(); return EXIT_SUCCESS;       break;
            case 'v': verbose = true;                          break;
            case 'f': filename = optarg;                       break;
            case '?': printf("Unknown option : %c\n", optopt); break;
            default :                                          break;
        }
    }
    if (verbose) printf("Starting.\n");
    if (optind < argc) filename = argv[argc-1];
    if (filename == NULL) {
        printf("Error, no input files\n");
        return EXIT_FAILURE;
    }
    if (verbose) printf("Loading file : %s\n", filename);
    char* program = load_file(filename);
    if (program == NULL) {
        if (verbose) printf("Exiting\n");
        return EXIT_FAILURE;
    }
    
    init_parser(verbose);
    //lex
    if (verbose) printf("Lexing.\n");
    vector* tokens = new_vector(0, sizeof(s_token));
    tokens->type = "s_token";
    lex(tokens, program);

    //parse
    if (verbose) printf("Parsing.\n");
    vector* parse_tree = new_vector(0, sizeof(pred_def));
    enum e_errors parse_res = parse(tokens, parse_tree);
    if (parse_res != 0) {
        printf("Parsing failed!\n%s\n", error_to_string(parse_res));
        return EXIT_FAILURE;
    }
    
    init_runtime(verbose);
    if (verbose) printf("Building Predicate Heads.\n");
    build_heads(parse_tree, database);
    if (verbose) printf("Building Predicate Bodies.\n");
    build_bodies(parse_tree, database);
    close_parser(parse_tree, verbose);//slight overlap with parser/runtime

    if (verbose) printf("Running headless predicates.\n");
    run(database);
    
    if (verbose) printf("Done.\n");
    
    free_vector(tokens);
    close_runtime(verbose);

    return EXIT_SUCCESS;
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

char* load_file(const char* filename) {
    char* buffer = NULL;
    long length;
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error! Could not open file : %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    rewind(file);

    buffer = malloc(length);
    if (buffer == NULL) {
        printf("Error! Could not allocate buffer for file : %s\n", filename);
        return NULL;
    }
    fread(buffer, length, 1, file);
    fclose(file);
    
    return buffer;
}
