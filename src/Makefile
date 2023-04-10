# We support 2 types of builds: Amiga and non-Amiga (Mac/Linux)
ifdef AMIGA
CC=vc +kick13
ASM=vasmm68k_mot -Fhunk -I$(NDK_ASMINC)
SYS_OBJECTS=amiga/display.o amiga/input.o amiga/memory.o amiga/splash_screen.o amiga/engine.o amiga/audio.o amiga/scenes.o ../ptplayer/ptplayer.o
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

TEST_OBJECTS=test/timer_test.o timers.o ../chibi_test/chibi.o
ENGINE_OBJECTS=main.o engine.o timers.o events.o memory.o audio.o display.o input.o physics.o resources.o scenes.o scripting.o $(SYS_OBJECTS)

.PHONY : clean check
.SUFFIXES : .o .c .asm

all: main

check: $(TEST_PRGS)

clean:
	rm -f $(ENGINE_OBJECTS) $(SYS_OBJECTS) $(EXES) $(TEST_OBJECTS) $(TEST_PRGS)

.c.o:
	$(CC) $(CFLAGS) $^ -c -o $@

.asm.o:
	$(ASM) $^ -o $@

main: $(ENGINE_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

#
# TESTS
#

#chibi.o: ../chibi_test/chibi.c
#	$(CC) $(TEST_CFLAGS) -c -o $@ $^

timer_test: $(TEST_OBJECTS)
	$(CC) -o $@ $^

#timer_test.o: test/timer_test.c
#	$(CC) $(TEST_CFLAGS) -c -o $@ $^
