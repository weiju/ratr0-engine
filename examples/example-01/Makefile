include ../Makefile.inc

EXES=example-01_game
TEST_PRGS=

# programs for benchmarks
PERF_PRGS=set_perf

TEST_OBJECTS=../chibi_test/chibi.o

# only what we need

# data structures and algorithms
DATA_OBJECTS=../../src/datastructs/bitset.o

ENGINE_OBJECTS=../../src/engine.o ../../src/timers.o ../../src/memory.o ../../src/input.o \
	../../src/resources.o ../../src/stages.o $(DATA_OBJECTS) $(HW_OBJECTS) $(EXT_OBJECTS)

# game objects
EXAMPLE01_OBJECTS=default_copper.o main.o main_scene.o

all: example-01_game

check: $(TEST_PRGS)

perf: $(PERF_PRGS)

clean:
	rm -f *.o $(ENGINE_OBJECTS)$(EXES) $(TEST_OBJECTS) $(TEST_PRGS) \
	default_copper.*

example-01_game: $(ENGINE_OBJECTS) $(EXAMPLE01_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

default_copper.c: default.copper
	ratr0-makecoplist --listname default_copper default.copper default_copper.c

#
# TESTS
#
