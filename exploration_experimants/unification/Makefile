
CC = gcc
LL = lex
YY = yacc
WARNS = -Wreturn-type
DEBUG_FLAGS = -DUNIFY_DEBUG
C_LIBS = y.tab.c lex.yy.c internal_struct.c generic_vector.c generic_map.c\
         mlog_array.c utils.c explication.c unify.c builtin_calls.c debug_stuff.c

.PHONY: run clean

all: objs
	$(CC) $(WARNS) *.o main.c -o build.bin

run: all
	./build.bin

debug_run: debug
	gdb debug.bin

debug: yaccify lexify $(C_LIBS)
	#$(YY) --debug --verbose -d parse.yacc
	$(YY) -d parse.yacc
	$(CC) $(WARNS) -g -O0 $(DEBUG_FLAGS) -c $(C_LIBS)
	$(CC) $(WARNS) -g -O0 $(DEBUG_FLAGS) *.o main.c -o debug.bin

objs: yaccify lexify $(C_LIBS)
	#$(CC) $(WARNS) -c $(C_LIBS)
	$(CC) $(WARNS) $(DEBUG_FLAGS) -c $(C_LIBS)

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
