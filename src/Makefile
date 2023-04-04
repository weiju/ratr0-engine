# We support 2 types of builds: Amiga and non-Amiga (Mac/Linux)
ifdef AMIGA
CC=vc +kick13
ASM=vasmm68k_mot -Fhunk -I$(NDK_ASMINC)
SYS_OBJECTS=amiga/display.o
CFLAGS=-I$(NDK_INC) -DDEBUG -DAMIGA -c99 -O2 -Iinclude
LDFLAGS=-lamiga -lauto
TEST_CFLAGS=-Iinclude -I../chibi_test
else
CC=gcc
SYS_OBJECTS=
CFLAGS=-Iinclude -DDEBUG -DUSE_SDL2
LDFLAGS=-lsdl2
TEST_CFLAGS=-Iinclude -I../chibi_test
endif  # ifdef AMIGA 

EXES=main
TEST_PRGS=timer_test

ENGINE_OBJECTS=main.o engine.o timers.o events.o memory.o audio.o display.o input.o physics.o resources.o scripting.o $(SYS_OBJECTS)

.PHONY : clean check
.SUFFIXES : .o .c

all: main

check: $(TEST_PRGS)

clean:
	rm -f $(ENGINE_OBJECTS) $(EXES) $(TEST_PRGS)

.c.o:
	$(CC) $(CFLAGS) $^ -c -o $@

main: $(ENGINE_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

#
# TESTS
#

chibi.o: ../chibi_test/chibi.c
	$(CC) $(TEST_CFLAGS) -c -o $@ $^

timer_test: timer_test.o timers.o chibi.o
	$(CC) -o $@ $^

timer_test.o: test/timer_test.c
	$(CC) $(TEST_CFLAGS) -c -o $@ $^
