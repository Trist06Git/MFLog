%{
    #include <stdio.h>
    #include "y.tab.h"
%}

%%

[0-9]+ {
    yylval.number = atoi(yytext);;
    return NUMBER;
}

[a-zA-Z][a-zA-Z0-9]* {
    yylval.string = strdup(yytext);
    return WORD;
}

":-"  return NECK;
['.'] return END;
[','] return AND;

":="  return ASSIGN;
['='] return EQUAL;

[\t | ' '] ;


[\n] ;

. return yytext[0];

%%

int yywrap() {
    return 1;
}
