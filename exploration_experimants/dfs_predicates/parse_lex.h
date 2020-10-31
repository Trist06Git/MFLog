#ifndef PARSE_LEX_H
#define PARSE_LEX_H

#include <stdbool.h>

#include "parse_lex_struct.h"
#include "generic_vector.h"

bool match_toke(char*, char*);
void parse_string(vector*, char*);

void dump_tokens(vector*);
const char* token_to_string(s_token*);

#endif