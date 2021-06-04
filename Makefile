
CC = gcc
LL = lex
YY = yacc
WARNS = -Wreturn-type
DEBUG_FLAGS = -DUNIFY_DEBUG
OPTIMISATIONS = -O3
C_LIBS = glob.tab.c lex.glob.c func.tab.c lex.func.c internal_struct.c generic_vector.c generic_map.c\
         mlog_array.c utils.c explication.c unify.c builtin_calls.c debug_stuff.c

.PHONY: run clean

all: objs
	$(CC) $(WARNS) $(OPTIMISATIONS) *.o main.c -o build.bin

run: all
	./build.bin

debug_run: debug
	gdb debug.bin

debug: debug_yacclex $(C_LIBS)
	$(CC) $(WARNS) -g -O0 $(DEBUG_FLAGS) -c $(C_LIBS)
	$(CC) $(WARNS) -g -O0 $(DEBUG_FLAGS) *.o main.c -o debug.bin

debug_yacclex: func_parse.yacc glob_parse.yacc
	#$(YY) --debug --verbose -d parse.yacc
	$(LL) --debug func_tokens.lex
	$(LL) --debug glob_tokens.lex
	$(YY) --debug --defines=func.tab.h -o func.tab.c -Wno-yacc func_parse.yacc
	$(YY) --debug --defines=glob.tab.h -o glob.tab.c -Wno-yacc glob_parse.yacc

objs: yaccify lexify $(C_LIBS)
	#$(CC) $(WARNS) -c $(C_LIBS)
	$(CC) $(WARNS) $(OPTIMISATIONS) $(DEBUG_FLAGS) -c $(C_LIBS)

lexify: yaccify func_tokens.lex glob_tokens.lex
	$(LL) func_tokens.lex
	$(LL) glob_tokens.lex

yaccify: func_parse.yacc glob_parse.yacc
	$(YY) --defines=func.tab.h -o func.tab.c -Wno-yacc func_parse.yacc
	$(YY) --defines=glob.tab.h -o glob.tab.c -Wno-yacc glob_parse.yacc

clean:
	-rm *.o
	-rm build.bin
	-rm lex.func.c lex.glob.c
	-rm func.tab.c func.tab.h
	-rm glob.tab.c glob.tab.h
	-rm y.output
	-rm debug.bin
