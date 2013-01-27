CC=gcc
CFLAGS=-c -Wall

all: shell test

shell: shell.o sllist.o
	$(CC) shell.o sllist.o -o shell

shell.o: shell.c
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
	rm -rf *.o *~ tests/*.o tests/*~ shell test
