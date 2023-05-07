# RATR0 Engine

## Description

A game engine in C for creating retro style 2D games. The goal is to
have a platform that works on both modern systems down to 16 and 8 bit
machines.

On modern systems, RATR0 will build on top of SDL, while on retro systems
it will be direct hardware abstraction.

## Restrictions

Since the plan is to target legacy architectures, at the core almost all
math and numbers will be integer-based. That means, coordinates, dimensions,
color values etc. are all integers. During physics calculations, some
fixed point math will be possible to allow for smoother animations and movement

## Architecture

The engine implements the standard architecture of having a collection
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

Below are the subsystems of the engine. In general every subsystem aims to be
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

### Display subsystem

The central component to implement all visual aspects of a game. In
RATR0 the goal is to achieve reasonably good rendering performance
on vintage computer systems like the Amiga by taking advantage of a
system's hardware features.

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

### [Physics / Collision subsystem]

The initial versions of RATR0 won't include a physics system.

## The World subsystem

### Scenes and Nodes

The world subsystem is based around scene trees. At each moment, there is a current
scene with a root node.
Nodes are communicating down the hierarchy by passing arguments to function calls and we use signals
to communicate up or across the hierarchy.

## Building the Engine

$ make

### Building for Amiga

$ make AMIGA=1
