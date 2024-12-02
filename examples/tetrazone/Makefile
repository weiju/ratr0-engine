include ../Makefile.inc

EXES=tetrazone__game
TEST_PRGS=tetris_test

# programs for benchmarks
PERF_PRGS=set_perf

TEST_OBJECTS=test/tetris_test.o ../chibi_test/chibi.o

# only what we need

# data structures and algorithms
DATA_OBJECTS=../../src/datastructs/bitset.o

ENGINE_OBJECTS=../../src/engine.o ../../src/timers.o ../../src/memory.o ../../src/input.o \
	../../src/resources.o ../../src/stages.o $(DATA_OBJECTS) $(HW_OBJECTS) $(EXT_OBJECTS)

# game objects
TETRAZONE_OBJECTS=default_copper.o tetris_copper.o tetris.o main_stage.o \
title_screen.o hiscore_screen.o game_data.o utils.o \
draw_primitives.o render_display.o fader.o

all: tetrazone_game

check: $(TEST_PRGS)

perf: $(PERF_PRGS)

clean:
	rm -f *.o $(ENGINE_OBJECTS)$(EXES) $(TEST_OBJECTS) $(TEST_PRGS) \
	default_copper.*

tetrazone_game: $(ENGINE_OBJECTS) $(TETRAZONE_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
ifdef RELEASE
	mkdir -p tetrazone_release/assets
	cp $@ tetrazone_game.info tetrazone_release
	cp -r assets/* tetrazone_release/assets
    # should go into a relase directory
	#lha a tetrazone.lha tetrazone_release.info tetrazone_release
endif

default_copper.c: default.copper
	ratr0-makecoplist --listname default_copper default.copper default_copper.c

tetris_copper.c: tetris.copper
	ratr0-makecoplist --listname tetris_copper tetris.copper tetris_copper.c

#
# TESTS
#
tetris_test: test/tetris_test.o game_data.o utils.o ../../chibi_test/chibi.o
	$(CC) $(LDFLAGS) -o $@ $^
