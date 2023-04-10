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
  * Scripting
  * Networking

The conceptual model is strongly inspired by the Godot Engine in that
we use scene graphs and everything is a node, so building a games is
essentially building a number of scene graphs. Like in Godot, we communicate
down the hierarchy by passing arguments to function calls and we use signals
to communicate up or across the hierarchy

## The subsystems

Below are the subsystems of the engine. In general every subsystem aims to be
updated once per frame, assuming a frame rate of 60fps, or 50 on retro systems
in the PAL region.
Each subsystem has a globally available service object that can be used to
access its functionality.

### Memory subsystem

### Display subsystem

### Event subsystem

### Timer subsystem

### Resource / File subsystem

### Audio  subsystem

### Input subsystem

### Physics / Collision subsystem

### Networking subsystem

### Scripting subsystem



## The game model

### Scenes and Nodes


### Scripts

