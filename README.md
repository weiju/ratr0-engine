# RATR0 Game Development Framework

## Description

A game development framework written in C for creating retro style 2D games.
The goal is to have a platform that works on both modern systems down to 16
and 8 bit machines.

On modern systems, RATR0 will build on top of SDL, while on retro systems
it will be direct hardware abstraction.

## Restrictions

Since the plan is to target legacy architectures, at the core almost all
math and numbers will be integer-based. That means, coordinates, dimensions,
color values etc. are all integers. During physics calculations, some
fixed point math will be possible to allow for smoother animations and movement

## Architecture

RATR0 implements a standard game engine architecture of having a collection
of subsystems that are tied and coordinated by the engine.
The engine is responsible for starting and shutting down all of the
sub systems.
The general concept is that each system is initialized with a `*_startup()`
function that returns an interface pointer. Each interface of a system
also contains a shutdown() function to cleanup the sub sytem.

All subsystems should not depend on each other's existence on startup(), with the
exception of Memory, which is so fundamental to the operation of the subsystems,
that it is the first subsystem that is initialized and every other subsystem
can safely use it at any time.

The following subsystems are planned

  * Memory
  * Events
  * Timers
  * Audio
  * Display
  * Input
  * Physics/Collisions
  * Resources and files
  * World simulation

## The subsystems

Below are the subsystems of RATR0. In general every subsystem aims to be
updated once per frame, assuming a frame rate of 60fps, or 50 on retro systems
in the PAL region.
Each subsystem has a globally available service object that can be used to
access its functionality.

### Memory subsystem

This system is where all other subsystem allocate their memory from.
By this we can ensure there is only a single point in the system where
memory is held and freed from the operating system.
This has the following benefits:

  * Testability: we can test subsystems that depend on the memory
    subsystem by injecting a test specific allocator
  * system independence: It can be beneficial to allocate memory through
    OS specific allocation methods rather than standard C ones to e.g.
    obtain memory with specific properties like chip memory in the Amiga
  * The memory subsystem can implement more efficient allocation strategies
    than the operating system that also can reduce fragmentation

### Rendering subsystem

The central component to implement all visual aspects of a game. In
RATR0 the goal is to achieve reasonably good rendering performance
on vintage computer systems like the Amiga by taking advantage of a
system's hardware features. Go [here](docs/RENDERING.md) for more information.

#### Tile data

TODO

#### BOBS

BOB animation are stored as tile sets that are layed out in a specific format.
To keep things simple and consistent, animation frames are stored in a single
column, and the right side is padded so there are at least 16 pixels of space
at the right side.
Each frame is aligned to the left side and the image width is a multiple of
16 pixels. This format is in accordance to the operation of the Amiga Blitter.

#### Hardware Sprites

Hardware sprite data is stored different from BOB data - while the frames
are still layed out as a column, the image is always 16 pixels wide and
2 frames are separated through a pair of 16 bit words.

### Event subsystem

### Timer subsystem

Timers are an essential tool in a game engine. They facilitate the
implementation of time-dependent behaviors that improve the playability
of a game.

### Resource / File subsystem

A platform independent way to interactive with the operating system's file
system to e.g. load game assets or load and store persistent data

### Audio  subsystem

A service to play background music and sound effects

### Input subsystem

Go [here](docs/INPUT.md) for more information

### [Physics / Collision subsystem]

Go [here](docs/COLLISIONS.md) for more information

## The World subsystem

Games are build from a set of scenes that are connected to each other. Each scene
represents an isolated logical unit like a level, a title screen or a high score
screen etc. Go [here](docs/WORLD.md) for more information.

### Scenes and Nodes

The world subsystem is based around scene trees. At each moment, there is a current
scene with a root node.
Nodes are communicating down the hierarchy by passing arguments to function calls and we use signals
to communicate up or across the hierarchy.

## Building the system

$ make

### Building for Amiga

$ make AMIGA=1
