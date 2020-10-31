#ifndef PARSE_LEX_H
#define PARSE_LEX_H

#include <stdbool.h>

#include "parse_lex_struct.h"
#include "token_vector_struct.h"

bool match_toke(char*, char*);
void parse_string(token_vector*, char*);

void dump_tokens(token_vector*);
const char* token_to_string(s_token*);

#endif