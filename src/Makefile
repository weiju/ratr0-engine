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

EXES=example-01_game invaders_game centipede_game tetrazone_game
TEST_PRGS=timer_test fixed_point_test bitset_test treeset_test quadtree_test vector_test queue_test tetris_test

# programs for benchmarks
PERF_PRGS=set_perf

TEST_OBJECTS=test/timer_test.o timers.o test/fixed_point_test.o \
	test/bitset_test.o test/treeset_test.o test/quadtree_test.o \
	test/vector_test.o test/queue_test.o test/tetris_test.o \
	../chibi_test/chibi.o

# only what we need

# data structures and algorithms
DATA_OBJECTS=datastructs/bitset.o

ENGINE_OBJECTS=engine.o timers.o memory.o input.o \
	resources.o stages.o default_copper.o $(DATA_OBJECTS) $(HW_OBJECTS) $(EXT_OBJECTS)

# game objects
EXAMPLE01_OBJECTS=example-01/main.o example-01/main_scene.o

INVADERS_OBJECTS=sinvaders/invaders.o sinvaders/inv_main_stage.o

CENTIPEDE_OBJECTS=centipede/centipede.o centipede/centipede_copper.o centipede/main_stage.o

TETRAZONE_OBJECTS=tetris/tetris.o tetris/tetris_copper.o tetris/main_stage.o \
tetris/title_screen.o tetris/hiscore_screen.o tetris/game_data.o tetris/utils.o \
tetris/draw_primitives.o tetris/render_display.o tetris/fader.o

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
	./tetris_test

perf: $(PERF_PRGS)

clean:
	rm -f *.o datastructs/*.o sinvaders/*.o example-01/*.o $(EXES) $(TEST_OBJECTS) $(TEST_PRGS) $(PERF_PRGS) \
		centipede/*.o centipede/centipede_copper.* default_copper.* \
		tetris/*.o tetris/tetris_copper.*

.c.o:
	$(CC) $(CFLAGS) $^ -c -o $@

.asm.o:
	$(ASM) $^ -o $@

test_startup: test_startup.o
	$(CC) $(LDFLAGS) -o $@ $^

test_console: test_console.o
	$(CC) $(LDFLAGS) -o $@ $^

example-01_game: $(ENGINE_OBJECTS) $(EXAMPLE01_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

invaders_game: $(ENGINE_OBJECTS) $(INVADERS_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

centipede_game: $(ENGINE_OBJECTS) $(CENTIPEDE_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

tetrazone_game: $(ENGINE_OBJECTS) $(TETRAZONE_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
ifdef RELEASE
	mkdir -p tetrazone_release/assets
	cp $@ tetrazone_game.info tetrazone_release
	cp -r tetris/assets/* tetrazone_release/assets
#	cp tetris/soundtrack/onlyamiga.mod tetrazone_release/assets
	lha a tetrazone.lha tetrazone_release.info tetrazone_release
endif


default_copper.c: default.copper
	ratr0-makecoplist default.copper default_copper.c

centipede/centipede_copper.c: centipede/centipede.copper
	ratr0-makecoplist --listname centi_copper centipede/centipede.copper centipede/centipede_copper.c

tetris/tetris_copper.c: tetris/tetris.copper
	ratr0-makecoplist --listname tetris_copper tetris/tetris.copper tetris/tetris_copper.c

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

tetris_test: test/tetris_test.o tetris/game_data.o tetris/utils.o ../chibi_test/chibi.o
	$(CC) $(LDFLAGS) -o $@ $^
