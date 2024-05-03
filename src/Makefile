ifdef TESTONLY

CC=gcc
CFLAGS=-I/opt/homebrew/include -Iinclude -DDEBUG
LDFLAGS=-L/opt/homebrew/lib -lsdl2 -lsdl2_image
TEST_CFLAGS=-Iinclude -I../chibi_test

else

CC=vc +kick13
ASM=vasmm68k_mot -Fhunk -I$(NDK_ASMINC)

HW_OBJECTS=amiga/display.o amiga/sprites.o amiga/blitter.o amiga/audio.o
OS_OBJECTS=amiga/input.o amiga/memory.o
EXT_OBJECTS=../ptplayer/ptplayer.o

CFLAGS=-I$(NDK_INC) -DDEBUG -DAMIGA -c99 -O2 -Iinclude
LDFLAGS=-lamiga -lauto
TEST_CFLAGS=-Iinclude -I../chibi_test

endif  # ifdef AMIGA

EXES=main invaders
TEST_PRGS=timer_test fixed_point_test bitset_test treeset_test quadtree_test vector_test

# programs for benchmarks
PERF_PRGS=set_perf

TEST_OBJECTS=test/timer_test.o timers.o test/fixed_point_test.o \
	test/bitset_test.o test/treeset_test.o test/quadtree_test.o \
	test/vector_test.o ../chibi_test/chibi.o

# only what we need

# data structures and algorithms
DATA_OBJECTS=datastructs/bitset.o

ENGINE_OBJECTS=engine.o timers.o memory.o audio.o rendering.o input.o \
	resources.o scenes.o $(DATA_OBJECTS) $(HW_OBJECTS) $(OS_OBJECTS) $(EXT_OBJECTS)

# game objects
MAIN_OBJECTS=example-01/main.o example-01/main_scene.o

INVADERS_OBJECTS=sinvaders/invaders.o sinvaders/inv_main_stage.o

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
	rm -f *.o datastructs/*.o amiga/*.o sinvaders/*.o example-01/*.o $(EXES) $(TEST_OBJECTS) $(TEST_PRGS) $(PERF_PRGS)

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

bitset_test: test/bitset_test.o datastructs/bitset.o ../chibi_test/chibi.o
	$(CC) -o $@ $^

treeset_test: test/treeset_test.o datastructs/treeset.o ../chibi_test/chibi.o
	$(CC) -o $@ $^

quadtree_test: test/quadtree_test.o datastructs/vector.o datastructs/quadtree.o ../chibi_test/chibi.o
	$(CC) -o $@ $^

vector_test: test/vector_test.o datastructs/vector.o ../chibi_test/chibi.o
	$(CC) -o $@ $^
