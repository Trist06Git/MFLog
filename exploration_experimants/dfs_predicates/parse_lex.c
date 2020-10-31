
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "parse_lex.h"
//#include "token_vector.h"
#include "generic_vector.h"

#define nl printf("\n")

char* test_prog = "pred1 :- pred2, pred3.\npred2 :- true.\npred3 :- true.\n";

int main(void) {
    printf("Starting.\n");
    printf("sizeof char     : %li\n", sizeof(char));
    printf("sizeof char*    : %li\n", sizeof(char*));
    printf("sizeof s_token  : %li\n", sizeof(s_token));
    printf("sizeof s_token* : %li\n", sizeof(s_token*));
    nl;

    vector* tokens = new_vector(0, sizeof(s_token));
    tokens->type = "s_token";
    parse_string(tokens, test_prog);
    dump_tokens(tokens);

    printf("Done.\n");
    return EXIT_SUCCESS;
}

bool match_token(char* str, char* toke) {
    bool res = true;
    for (int i = 0; toke[i]; i++) {
        res &= str[i] == toke[i];
    }
    return res;
}

bool is_alpha_numer(char c) {
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9');
}

bool match_word(char* str, char** word, int* count) {
    for (; str[*count] && is_alpha_numer(str[*count]); (*count)++);

    *word = malloc(sizeof(char)*((*count)+1));//leak if no alpha found
    memcpy(*word, str, sizeof(char)*(*count));
    *(*word+*count) = '\0';//something to do with double pointer != pointer[], or some such

    return (*count > 0);
}

//thing:-thing, thing.
void parse_string(vector* tokens, char* str) {
    while (str[0]) {
        bool match = false;
        char* word = NULL;
        int word_size = 0;
        if (match_token(str, ":-")) {
            match = true;
            push_back(tokens, &(s_token){.tag = NECK, .name = ""});
            str += 2;
        } else if (match_token(str, ".")) {
            match = true;
            push_back(tokens, &(s_token){.tag = END_PRED, .name = ""});
            str += 1;
        } else if (match_token(str, ",")) {
            match = true;
            push_back(tokens, &(s_token){.tag = AND, .name = ""});
            str += 1;
        } else if (match_word(str, &word, &word_size)) {
            push_back(tokens, &(s_token){.tag = PRED, .name = word});
            match = true;
            str += word_size;
        }
        if (!match) str++;
    }
}

void dump_tokens(vector* tokens) {
    printf("Dumping tokens :\n");
    for (int i = 0; i < tokens->count; i++) {
        printf("%s\n", token_to_string((s_token*)at(tokens, i)));
    }
    nl;
}

const char* token_to_string(s_token* token) {
    char* ret = malloc(sizeof(char)*80);
    switch (token->tag) {
        case PRED: {
            strcat(ret, "PRED(");
            strcat(ret, token->name);
            strcat(ret, ")");
            break;
        }
        case NECK    : strcat(ret, "NECK");     break;
        case AND     : strcat(ret, "AND");      break;
        case END_PRED: strcat(ret, "END_PRED"); break;
    }
    
    return ret;
}
