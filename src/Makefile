# We support 2 types of builds: Amiga and non-Amiga (Mac/Linux)
ifdef AMIGA
CC=vc +kick13
ASM=vasmm68k_mot -Fhunk -I$(NDK_ASMINC)
SYS_OBJECTS=amiga/display.o amiga/sprites.o amiga/blitter.o amiga/input.o amiga/memory.o \
	amiga/engine.o amiga/audio.o \
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

EXES=main invaders
PERF_PRGS=set_perf
TEST_PRGS=timer_test fixed_point_test bitset_test treeset_test quadtree_test vector_test

TEST_OBJECTS=test/timer_test.o timers.o test/fixed_point_test.o \
	test/bitset_test.o test/treeset_test.o test/quadtree_test.o \
	test/vector_test.o ../chibi_test/chibi.o

# everything
#ENGINE_OBJECTS=engine.o timers.o events.o memory.o audio.o rendering.o input.o \
#	resources.o scenes.o bitset.o hash_grid.o quadtree.o vector.o $(SYS_OBJECTS)

# only what we need
ENGINE_OBJECTS=engine.o timers.o memory.o audio.o rendering.o input.o \
	resources.o scenes.o bitset.o $(SYS_OBJECTS)

MAIN_OBJECTS=main.o main_scene.o

INVADERS_OBJECTS=invaders.o inv_main_stage.o

.PHONY : clean check
.SUFFIXES : .o .c .asm

all: main

check: $(TEST_PRGS)
	./timer_test
	./fixed_point_test
	./bitset_test
	./treeset_test
	./quadtree_test
	./vector_test

perf: $(PERF_PRGS)

clean:
	rm -f $(ENGINE_OBJECTS) $(SYS_OBJECTS) $(EXES) $(TEST_OBJECTS) $(TEST_PRGS) \
		$(MAIN_OBJECTS) $(INVADERS_OBJECTS) \
		$(PERF_OBJECTS) $(PERF_PRGS)

.c.o:
	$(CC) $(CFLAGS) $^ -c -o $@

.asm.o:
	$(ASM) $^ -o $@

main: $(ENGINE_OBJECTS) $(MAIN_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

invaders: $(ENGINE_OBJECTS) $(INVADERS_OBJECTS)
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

treeset_test: test/treeset_test.o treeset.o ../chibi_test/chibi.o
	$(CC) -o $@ $^

quadtree_test: test/quadtree_test.o vector.o quadtree.o ../chibi_test/chibi.o
	$(CC) -o $@ $^

vector_test: test/vector_test.o vector.o ../chibi_test/chibi.o
	$(CC) -o $@ $^
