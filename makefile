CC = gcc

phantom:
	$(CC) token.c lexer.c main.c -o phantom.exe

test:
	$(CC) token.c lexer.c test.c -o tests.exe

#phantom: main.o lexer.o
#	$(#CC) main.o lexer.o -o phantom.exe

#main.o: main.c
#	$(#CC) -c main.c

#lexer.o: lexer.h lexer.c
#	$(#CC) lexer.h lexer.c token.o

#token.o: token.h token.c
#	$(#CC) -c token.h token.c

clean:
	rm *.exe *.gch
