%{
    #include <stdio.h>
    int flag = 0;
%}

%token NUMBER
%left '+' '-'
%left '*' '/' '%'
%left '(' ')'

%%

Expr: E {
  printf("\nResult = %d\n", $$);  
  return 0;
};

E: E '+' E   { $$ = $1 + $3; }
 | E '-' E   { $$ = $1 - $3; }
 | E '*' E   { $$ = $1 * $3; }
 | E '/' E   { $$ = $1 / $3; }
 | E '%' E   { $$ = $1 % $3; }
 | '(' E ')' { $$ = $2; }
 | NUMBER    { $$ = $1; }
 ;

%%

int main() {
    printf("Calculator app, type a 5 function maths expression:\n");
    yyparse();
    if (flag == 1) printf("\nInvalid expression!\n");
    return 0;
}

void yyerror() {
    flag = 1;
}
