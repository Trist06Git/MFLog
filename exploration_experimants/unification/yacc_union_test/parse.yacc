%{
    #include <stdio.h>
    #include "some_struct.h"

    extern int flag;
    extern super parsed;

    int yylex(void);
    void yyerror(char*);
%}

%start Program

%union {
    char* string;
    int number;
    assign asgn;
}

%type <asgn> Assign

%token <number> NUMBER
%token <string> WORD
%token EQUAL
%token NL

%%

Program : Line
        | Line Program
        ;

Line : WORD Assign {
    parsed.name = $1;
    parsed.var = $2;
}
;

Assign : WORD EQUAL NUMBER {
    assign a;
    a.name = $1;
    a.val = $3;
    $$ = a;
}
;

/*
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
*/

%%

void yyerror(char* s) {
    flag = 1;
}
