#ifndef PARSE_LEX_STRUCT_H
#define PARSE_LEX_STRUCT_H

//THIS IS OLD, DO NOT USE

enum e_token {PRED, NECK, AND, END_PRED, ERROR};
typedef struct s_token {
    enum e_token tag;
    char* name;
} s_token;

#endif