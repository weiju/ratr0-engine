# We support 2 types of builds: Amiga and non-Amiga (Mac/Linux)
ifdef AMIGA
CC=vc +kick13
ASM=vasmm68k_mot -Fhunk -I$(NDK_ASMINC)
SYS_OBJECTS=amiga/display.o amiga/sprites.o amiga/blitter.o amiga/input.o amiga/memory.o \
	amiga/engine.o amiga/audio.o amiga/world.o \
	../ptplayer/ptplayer.o
CFLAGS=-I$(NDK_INC) -DDEBUG -DAMIGA -c99 -O2 -Iinclude
LDFLAGS=-lamiga -lauto
TEST_CFLAGS=-Iinclude -I../chibi_test
else
CC=gcc
SYS_OBJECTS=SDL/engine.o
CFLAGS=-I/opt/homebrew/include -Iinclude -DDEBUG -DUSE_SDL2
LDFLAGS=-L/opt/homebrew/lib -lsdl2 -lsdl2_image
TEST_CFLAGS=-Iinclude -I../chibi_test
endif  # ifdef AMIGA

EXES=main
PERF_PRGS=set_perf
TEST_PRGS=timer_test fixed_point_test bitset_test

TEST_OBJECTS=test/timer_test.o timers.o test/fixed_point_test.o \
	test/bitset_test.o ../chibi_test/chibi.o

ENGINE_OBJECTS=main.o engine.o timers.o events.o memory.o audio.o rendering.o input.o \
	resources.o world.o bitset.o $(SYS_OBJECTS)

TEST_OBJECTS=benchmark/set_perf.o

.PHONY : clean check
.SUFFIXES : .o .c .asm

all: main

check: $(TEST_PRGS)

perf: $(PERF_PRGS)

clean:
	rm -f $(ENGINE_OBJECTS) $(SYS_OBJECTS) $(EXES) $(TEST_OBJECTS) $(TEST_PRGS) $(PERF_OBJECTS) $(PERF_PRGS)

.c.o:
	$(CC) $(CFLAGS) $^ -c -o $@

.asm.o:
	$(ASM) $^ -o $@

main: $(ENGINE_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

#
# TESTS
#
timer_test: test/timer_test.o timers.o ../chibi_test/chibi.o
	$(CC) -o $@ $^

fixed_point_test: test/fixed_point_test.o ../chibi_test/chibi.o
	$(CC) -o $@ $^

bitset_test: test/bitset_test.o bitset.o ../chibi_test/chibi.o
	$(CC) -o $@ $^

