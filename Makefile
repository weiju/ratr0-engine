CC = gcc
#CFLAGS=-I$(NDK_INC) -c99 -O2 -I../include
CFLAGS=

.PHONY : clean check
.SUFFIXES : .o .c

all: main

clean:
	rm -f *.o $(EXES)

.c.o:
	$(CC) $(CFLAGS) $^ -c -o $@

main: main.o timers.o
	$(CC) -o $@ $^
