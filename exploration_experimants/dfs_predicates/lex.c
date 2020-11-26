
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lex.h"
#include "generic_vector.h"

#define nl printf("\n")

char* pred_call = ":- true.";
char* test_single_def = "pred1 :- pred2.";
char* test_multi_pred = "pred1 :- pred2, pred3.";
char* test_prog = "pred1 :- pred2, pred3.\npred2 :- true.\npred3 :- true. :- pred1. :- pred3.\n";

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
    for (*count = 0; str[*count] && is_alpha_numer(str[*count]); (*count)++);

    if (*count > 0) {
        *word = malloc(sizeof(char)*((*count)+1));
        memcpy(*word, str, sizeof(char)*(*count));
        *(*word+*count) = '\0';//something to do with double pointer != pointer[], or some such
        return true;
    } else {
        return false;
    }
}

void lex(vector* tokens, char* str) {
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

//for debug
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
