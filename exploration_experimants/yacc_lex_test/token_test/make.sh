#!/bin/bash

yacc -d calc.yacc
lex calc.lex
gcc y.tab.c lex.yy.c

