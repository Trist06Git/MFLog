
CC = gcc
LL = lex
YY = yacc
C_LIBS = y.tab.c lex.yy.c internals.c

all: objs
	$(CC) *.o main.c -o build.bin

run: all
	./build.bin

objs: yaccify lexify $(C_LIBS)
	$(CC) -c $(C_LIBS)

lexify: yaccify tokens.lex
	$(LL) tokens.lex

yaccify: tokens.yacc
	$(YY) -d tokens.yacc

clean:
	-rm *.o
	-rm build.bin
	-rm lex.yy.c
	-rm y.tab.c y.tab.h
