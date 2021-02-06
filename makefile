CC = gcc
CFLAGS = -g -Wall
FILES = $(shell ls *.c)
#OBJS = ${FILES:%.c=%.o}#lexer.o debug.o
OBJS = lexer.o debug.o parser.o ast.o compiler.o vm.o hashtable.o

all: phantom

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $^

phantom: $(OBJS) main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm phantom *.o *.gch
