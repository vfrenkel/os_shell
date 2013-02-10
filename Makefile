CC=gcc
CFLAGS=-g -c -Wall -std=gnu99

all: myshell test

myshell: shell_main.o shell.o sllist.o
	$(CC) shell_main.o shell.o sllist.o -o myshell

shell_main.o: shell_main.c
	$(CC) $(CFLAGS) shell_main.c

shell.o: shell.h shell.c
	$(CC) $(CFLAGS) shell.c

sllist.o: sllist.h sllist.c
	$(CC) $(CFLAGS) sllist.c

test: tests/test.o tests/test_sllist.o tests/CuTest.o sllist.o
	$(CC) sllist.o tests/test_sllist.o tests/CuTest.o tests/test.o -o test

tests/test.o: tests/test.c
	$(CC) $(CFLAGS) tests/test.c -o tests/test.o

tests/test_sllist.o: tests/test_sllist.c
	$(CC) $(CFLAGS) tests/test_sllist.c -o tests/test_sllist.o

tests/CuTest.o: tests/CuTest.c
	$(CC) $(CFLAGS) tests/CuTest.c -o tests/CuTest.o

clean:
	rm -rf *.o *~ tests/*.o tests/*~ myshell test
