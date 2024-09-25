# RATR0 Game Development Framework

![Build Status](https://github.com/weiju/ratr0-engine/actions/workflows/c-cpp.yml/badge.svg)

## Description

A game development framework written in C for creating retro style 2D games on
the Amiga.

## Disclaimer

At the moment, I don't recommend writing any game based on RATR0. There will be
a lot of changes as my understanding of the concepts deepens.

What you *can* do though is study the examples and the code and take what you
find useful. Maybe you are struggling with a current technical problem
and you are looking for a solution in other people's code. That's what I do,
too. Hope you find something that helps.

There are alternatives you might want to consider:

Take a look at ACE (Amiga C Engine), if you want to use C. Or Scorpion if you
want to just focus on making a game - or whatever you like. These engines are
intended to be used by a wider audience and provide more support to
help you achive your goal.

## Why did I create RATR0 ?

RATR0 is all about learning: learning to understand the Amiga hardware
as it concerns game development, learning what goes on in a game engine
all the while creating fun, fast on the Amiga.

I wanted to create a way to facilitate game development for myself,
and enable me to work the way I want, which would be typically on a
modern Mac or Linux laptop.

I just enjoy programming for fun while I am outside in a coffee shop
or in the backyard.

## Why I chose C over assembly and C++

C for me represents an acceptable compromise as an implementation language.
It is high level enough to create maintainable systems, and also has
great compilers that support modern C language standards.
While there exist a modern GCC version for the Amiga, my main target platform
is Amiga OS 1.3, which is not necessarily easy to develop for with
GCC, so I decided that C++ on Amiga is not for me.
I used to write a lot of assembly code in my teens, but nowadays I try to
avoid using it. It allows for a great amount of control for sure, but
it invites the introduction of a lot of hard to find errors, and typically
it results in much more, much harder to understand code.

## Why a library ?

RATR0 is designed as a C library for the main reason that it is simple.
I tried to avoid making it too much of a framework, because it's hard
to predict what kind of games are supposed to be developed with it.
So this will start as a small library and the idea is that genre specific
enhancements can be built on top of it.

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

#### Sprites

Go [here](docs/SPRITES.md) for more information

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

### Testing

The unit tests are setup in a way that they can be run without
an Amiga system.

$ make TESTONLY=1 check

### Updating chibi_test

$ git submodule update --init --remote --recursive
$ git submodule update --recursive

## Closing words

I have my opinions and values and you have yours. That's fine. I don't
give a f@ck if they are different from mine.
Have fun, experiment, learn and write games, the way you want !
