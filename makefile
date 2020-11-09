CC = gcc
CFLAGS = -c -o
OBJS = main.o lexer.o debug.o parser.o

phantom: $(OBJS)
	$(CC) -o phantom $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) main.o main.c

lexer.o: lexer.c
	$(CC) $(CFLAGS) lexer.o lexer.c

parser.o: parser.c
	$(CC) $(CFLAGS) parser.o parser.c

debug.o: debug.c
	$(CC) $(CFLAGS) debug.o debug.c

clean:
	rm phantom *.o
