%{
    #include <stdio.h>
    #include "internal_struct.h"
    #include "y.tab.h"

    extern void yyerror(char*);

%}

%%

[0-9]+ {
    yylval.number = atoi(yytext);
    return NUMBER;
}

[a-zA-Z]+[a-zA-Z0-9]* {
    yylval.string = strdup(yytext);
    return WORD;
}

[(] return LP_ROUND;
[)] return RP_ROUND;

[,] return AND;
[=] return EQUAL;
[.] return END;

[/] return DIV;
[*] return MUL;
[+] return PLUS;
[-] return MINUS;

[\t] ;
" " ;

[\n] ;

<<EOF>> { 
    printf("End Of File\n");
    yyterminate();
};
. return yytext[0];

%%

int yywrap() {
    return 1;
}
