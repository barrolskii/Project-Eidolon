CC = gcc

all:
	$(CC) token.h main.c -o phantom.exe

clean:
	rm *.exe
