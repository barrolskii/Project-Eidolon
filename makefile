CC = gcc
CFLAGS = -c -o
OBJS = main.o lexer.o

phantom: $(OBJS)
	$(CC) -o phantom $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) main.o main.c

lexer.o: lexer.c
	$(CC) $(CFLAGS) lexer.o lexer.c

clean:
	rm phantom *.o
