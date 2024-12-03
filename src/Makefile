ifdef TESTONLY

CC=gcc
CFLAGS=-I/opt/homebrew/include -Iinclude -DDEBUG -DTEST
LDFLAGS=
TEST_CFLAGS=-Iinclude -I../chibi_test

else

CC=vc +kick13
ASM=vasmm68k_mot -Fhunk -I$(NDK_ASMINC)

HW_OBJECTS=display.o sprites.o blitter.o audio.o
EXT_OBJECTS=../ptplayer/ptplayer.o

ifdef RELEASE
CFLAGS=-I$(NDK_INC) -DAMIGA -c99 -O2 -Iinclude
else
CFLAGS=-I$(NDK_INC) -DDEBUG -DAMIGA -c99 -O2 -Iinclude
endif

# no optimizations so we can make sure it's not a compiler
# optimization related bug
#CFLAGS=-I$(NDK_INC) -DDEBUG -DAMIGA -c99 -Iinclude
LDFLAGS=-lamiga -lauto
TEST_CFLAGS=-Iinclude -I../chibi_test

endif  # ifdef AMIGA

TEST_PRGS=fixed_point_test bitset_test treeset_test quadtree_test vector_test queue_test timer_test

# programs for benchmarks
PERF_PRGS=set_perf

TEST_OBJECTS=test/timer_test.o timers.o test/fixed_point_test.o \
	test/bitset_test.o test/treeset_test.o test/quadtree_test.o \
	test/vector_test.o test/queue_test.o \
	../chibi_test/chibi.o

# only what we need

# data structures and algorithms
DATA_OBJECTS=datastructs/bitset.o

ENGINE_OBJECTS=engine.o timers.o memory.o input.o \
	resources.o stages.o $(DATA_OBJECTS) $(HW_OBJECTS) $(EXT_OBJECTS)

.PHONY : clean check
.SUFFIXES : .o .c .asm

all: example-01_game

check: $(TEST_PRGS)
	./timer_test
	./fixed_point_test
	./bitset_test
	./treeset_test
	./quadtree_test
	./vector_test
	./queue_test

perf: $(PERF_PRGS)

clean:
	rm -f *.o datastructs/*.o $(EXES) $(TEST_OBJECTS) $(TEST_PRGS) $(PERF_PRGS)

.c.o:
	$(CC) $(CFLAGS) $^ -c -o $@

.asm.o:
	$(ASM) $^ -o $@

test_startup: prototypes/test_startup.o
	$(CC) $(LDFLAGS) -o $@ $^

test_console: prototypes/test_console.o
	$(CC) $(LDFLAGS) -o $@ $^

#
# TESTS
#
test_queue: test_queue.o
	$(CC) $(LDFLAGS) -o $@ $^

queue_test: test/queue_test.o ../chibi_test/chibi.o
	$(CC) $(LDFLAGS) -o $@ $^

timer_test: test/timer_test.o timers.o ../chibi_test/chibi.o
	$(CC) $(LDFLAGS) -o $@ $^

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

