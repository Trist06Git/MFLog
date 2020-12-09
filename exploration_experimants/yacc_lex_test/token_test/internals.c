
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "internals.h"

/*
char* token_to_string(t_token token) {
    char* buffer = malloc(sizeof(char)*20);
    buffer[0] = '\0';
    switch (token.tag) {
        case E_WORD   : {
            sprintf(buffer, "word(%s)", token.u_token.w.word);
        } break;
        case E_NUMBER : {
            sprintf(buffer, "number(%d)", token.u_token.n.number);
        } break;
        case E_OTHER  : sprintf(buffer, "other"); break;
        default       : sprintf(buffer, "?");     break;
    }
    return buffer;
}
*/

/*
char* token_to_string(enum e_token t) {
    switch (t) {
        case E_WORD   : return "word";
        case E_NUMBER : return "number";
        case E_OTHER  : return "other";
        default       : return "?E?";
    }
}
*/
