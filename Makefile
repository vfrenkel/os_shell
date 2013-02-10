CC=gcc
CFLAGS=-c -Wall -std=gnu99

all: myshell

myshell: shell_main.o shell.o sllist.o
	$(CC) shell_main.o shell.o sllist.o -o myshell

shell_main.o: shell_main.c
	$(CC) $(CFLAGS) shell_main.c

shell.o: shell.h shell.c
	$(CC) $(CFLAGS) shell.c

sllist.o: sllist.h sllist.c
	$(CC) $(CFLAGS) sllist.c

clean:
	rm -rf *.o *~ myshell
