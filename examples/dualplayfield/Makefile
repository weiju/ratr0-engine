include ../Makefile.inc

EXES=dualplayfield
TEST_PRGS=

TEST_OBJECTS=../chibi_test/chibi.o

# only what we need

# data structures and algorithms
DATA_OBJECTS=../../src/datastructs/bitset.o

ENGINE_OBJECTS=../../src/engine.o ../../src/timers.o ../../src/memory.o ../../src/input.o \
	../../src/resources.o ../../src/stages.o $(DATA_OBJECTS) $(HW_OBJECTS) $(EXT_OBJECTS)

# game objects
DUALPLAYFIELD_OBJECTS=dualplayfield_copper.o dualplayfield.o dualplayfield_copper.o

all: dualplayfield_game

check: $(TEST_PRGS)

clean:
	rm -f *.o $(ENGINE_OBJECTS)$(EXES) $(TEST_OBJECTS) $(TEST_PRGS) \
	dualplayfield_copper.*

dualplayfield_game: $(ENGINE_OBJECTS) $(DUALPLAYFIELD_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

dualplayfield_copper.c: dualplayfield.copper
	ratr0-makecoplist --listname dualplayfield_copper dualplayfield.copper dualplayfield_copper.c

#
# TESTS
#
