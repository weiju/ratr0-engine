# The Memory subsystem

## Motivation

Any larger application can beneft from a dedicated memory management system.
Depending on the type of application, the size, amount and frequency of
allocating and freeing memory blocks can vary significantly.

In Amiga games, we often deal with allocating and freeing larger blocks of memory,
and if we do this too frequently at some point the operating system
will be unable to find suitable memory blocks due to fragmentation.

It therefore makes sense to preallocate a small number of larger memory
blocks at the start of the program and free them at the end of the program.
The game's memory allocator is responsible for managing all the game's objects.
This will ensure the game will unlikely run out of memory and also be able
to cleanly exit to the operating system.

