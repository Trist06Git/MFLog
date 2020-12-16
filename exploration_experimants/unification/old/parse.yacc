%{
    #include <stdio.h>
    #include "predicate.h"
    #include "generic_vector.h"
    extern int flag;
    extern vector* names;
    extern vector* preds;
    extern vector* calls;

    int yylex(void);
    void yyerror(char*);
%}

%start Program

%union {
    char* string;
    int number;
}

%token <number> NUMBER
%token <string> WORD
%token ASSIGN EQUAL
%token NECK END AND
%token NL

%%

Program : Line
        | Line Program
        ;

Line : Pred_Def
     | Pred_Call
     ;

Pred_Call : NECK Expr END {
    predicate p;
    p.name = "CALL";
    p.subs = names;
    push_back(calls, &p);
    names = new_vector(0, sizeof(char*));
};

Pred_Def : WORD NECK Expr END {
    predicate p;
    p.name = $1;
    p.subs = names;
    push_back(preds, &p);
    names = new_vector(0, sizeof(char*));
};

Expr : WORD AND Expr { push_back(names, $1); }
     | WORD          { push_back(names, $1); }
     ;

%%

void yyerror(char* s) {
    flag = 1;
}
