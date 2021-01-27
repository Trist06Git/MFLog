
CC = gcc
LL = lex
YY = yacc
C_LIBS = y.tab.c lex.yy.c internal_struct.c generic_vector.c generic_map.c utils.c explication.c

.PHONY: run clean

all: objs
	$(CC) *.o main.c -o build.bin

run: all
	./build.bin

debug_run: debug
	gdb debug.bin

debug: yaccify lexify $(C_LIBS)
	#$(YY) --debug --verbose -d parse.yacc
	$(YY) --debug -d parse.yacc
	$(CC) -g -O0 -c $(C_LIBS)
	$(CC) -g -O0 *.o main.c -o debug.bin

objs: yaccify lexify $(C_LIBS)
	$(CC) -c $(C_LIBS)

lexify: yaccify tokens.lex
	$(LL) tokens.lex

yaccify: parse.yacc
	$(YY) -d parse.yacc

clean:
	-rm *.o
	-rm build.bin
	-rm lex.yy.c
	-rm y.tab.c y.tab.h
	-rm y.output
	-rm debug.bin
