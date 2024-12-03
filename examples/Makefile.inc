ifdef TESTONLY

CC=gcc
CFLAGS=-I/opt/homebrew/include -I../../src/include -DDEBUG -DTEST
LDFLAGS=
TEST_CFLAGS=-Iinclude -I../../chibi_test

# programs for benchmarks
PERF_PRGS=set_perf


else

CC=vc +kick13
ASM=vasmm68k_mot -Fhunk -I$(NDK_ASMINC)

HW_OBJECTS=../../src/display.o ../../src/sprites.o ../../src/blitter.o ../../src/audio.o
EXT_OBJECTS=../../ptplayer/ptplayer.o

ifdef RELEASE
CFLAGS=-I$(NDK_INC) -DAMIGA -c99 -O2 -Iinclude
else
CFLAGS=-I$(NDK_INC) -DDEBUG -DAMIGA -c99 -O2 -I../../src/include
endif

# no optimizations so we can make sure it's not a compiler
# optimization related bug
#CFLAGS=-I$(NDK_INC) -DDEBUG -DAMIGA -c99 -Iinclude
LDFLAGS=-lamiga -lauto
TEST_CFLAGS=-Iinclude -I../chibi_test

endif  # ifdef AMIGA

.PHONY : clean check
.SUFFIXES : .o .c .asm

.c.o:
	$(CC) $(CFLAGS) $^ -c -o $@

.asm.o:
	$(ASM) $^ -o $@
