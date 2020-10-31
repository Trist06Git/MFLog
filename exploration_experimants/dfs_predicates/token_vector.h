#ifndef TOKEN_VECTOR_H
#define TOKEN_VECTOR_H

#include "token_vector_struct.h"
#include "parse_lex_struct.h"

void push_back(token_vector*, s_token);
s_token* at(token_vector*, int);
int size(token_vector*);
token_vector* new_token_vector(int);
void free_token_vector(token_vector*);//...

#endif