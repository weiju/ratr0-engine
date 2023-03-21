CC = gcc
#CFLAGS=-I$(NDK_INC) -c99 -O2 -I../include
CFLAGS=-Iinclude -DDEBUG
TEST_CFLAGS=-Iinclude -Ichibi_test
EXES=main
TEST_PRGS=timer_test


.PHONY : clean check
.SUFFIXES : .o .c

all: main

check: $(TEST_PRGS)

clean:
	rm -f *.o $(EXES) $(TEST_PRGS)

.c.o:
	$(CC) $(CFLAGS) $^ -c -o $@

main: main.o engine.o timers.o events.o memory.o audio.o
	$(CC) -o $@ $^


#
# TESTS
#

chibi.o: chibi_test/chibi.c
	$(CC) $(TEST_CFLAGS) -c -o $@ $^

timer_test: timer_test.o timers.o chibi.o
	$(CC) -o $@ $^

timer_test.o: test/timer_test.c
	$(CC) $(TEST_CFLAGS) -c -o $@ $^
