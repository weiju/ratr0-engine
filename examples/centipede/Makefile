include ../Makefile.inc

EXES=centipede_game
TEST_PRGS=

TEST_OBJECTS=../chibi_test/chibi.o

# only what we need

# data structures and algorithms
DATA_OBJECTS=../../src/datastructs/bitset.o

ENGINE_OBJECTS=../../src/engine.o ../../src/timers.o ../../src/memory.o ../../src/input.o \
	../../src/resources.o ../../src/stages.o $(DATA_OBJECTS) $(HW_OBJECTS) $(EXT_OBJECTS)

# game objects
CENTIPEDE_OBJECTS=centipede.o centipede_copper.o main_stage.o

all: centipede_game

check: $(TEST_PRGS)

perf: $(PERF_PRGS)

clean:
	rm -f *.o $(ENGINE_OBJECTS)$(EXES) $(TEST_OBJECTS) $(TEST_PRGS) \
	centipede_copper.*

centipede_game: $(ENGINE_OBJECTS) $(CENTIPEDE_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

centipede_copper.c: centipede.copper
	ratr0-makecoplist --listname centi_copper centipede.copper centipede_copper.c

#
# TESTS
#
