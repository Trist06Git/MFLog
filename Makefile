
CC = gcc
C_LIBS = mafialog_alloc.c

all: objs
	$(CC) *.o main.c -o build.bin

run: all
	./build.bin

objs: $(C_LIBS)
	$(CC) -c $(C_LIBS)

tests: objs
	$(CC) *.o tests/tests.c -o tests/tests.bin
	tests/tests.bin

clean:
	-rm *.o
	-rm build.bin
	-rm var_*
	-rm tests/tests.bin
	-rm tests/var_*
