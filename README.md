# RATR0 Engine

## Description

A game engine in C for creating retro style 2D games. The goal is to
have a platform that works on both modern systems down to 16 and 8 bit
machines.

On modern systems, RATR0 will build on top of SDL, while on retro systems
it will be direct hardware abstraction.

## Architecture

The engine implements the standard architecture of having a collection
of subsystems that are tied and coordinated in a larger system.


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
in the PAL region

### Memory subsystem

### Event subsystem

### Timer subsystem

### Resource / File subsystem

### Display subsystem

### Audio  subsystem

### Input subsystem

### Physics / Collision subsystem

### Networking subsystem

### Scripting subsystem



## The game model

### Scenes and Nodes


### Scripts

