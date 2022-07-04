CC = gcc
all: tokenizer.o shell.o signal_handler.o
	$(CC) tokenizer.o shell.o signal_handler.o -o shell
tokenizer.o: tokenizer.h tokenizer.c
	$(CC) -c tokenizer.c
shell.o: shell.c
	$(CC) -c shell.c
signal_handler.o:
	$(CC) -c signal_handler.c	
clean:
	rm *.o shell	