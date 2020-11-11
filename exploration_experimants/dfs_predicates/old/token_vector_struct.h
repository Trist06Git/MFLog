#ifndef TOKEN_VECTOR_STRUCT_H
#define TOKEN_VECTOR_STRUCT_H

#include "parse_lex_struct.h"

typedef struct token_vector {
    int count;
    int allocated;
    s_token* store;
} token_vector;

#endif