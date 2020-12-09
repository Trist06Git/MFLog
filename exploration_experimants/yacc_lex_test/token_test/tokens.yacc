%{
    #include <stdio.h>
    #include "internals.h"
    extern int flag;

    extern char* name;
    extern int val;

    int yylex(void);
    void yyerror(char* s);
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

Program : Pred_Def  {printf("Definition\n");}
        | Pred_Call {printf("Call\n");}
        ;

Pred_Call : NECK Expr END { ; }
          ;

Pred_Def : WORD NECK Expr END {printf("%s :-\n", $1);}
         ;

Expr : WORD AND Expr {printf("%s ,\n", $1);}
     | WORD          {printf("%s\n", $1);}
     ;

/*
Assign : WORD ASSIGN NUMBER {
    name = $1;
    val = $3;
}
       ;

Expr : Expr Expr
     | Assign { ; }
     | NL     { return 0; }
     ;

E : E E
  | WORD   { printf("word : %s\n", $1); }
  | NUMBER { printf("number : %d\n", $1); }
  ;
*/

%%

void yyerror(char* s) {
    flag = 1;
}
