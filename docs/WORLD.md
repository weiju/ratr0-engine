# World system

## Description

The world system is designed around scene objects. Scene object are containers
that can be automatically inspected and run by the world subsystem.
Only one scene can be active at any given moment, and it could represent a
level, or a user interface, like e.g. a title screen.

## Scenes

A Scene is a container for game objects. The world system inspects the active
scene in every iteration of the game loop. The most important objects in the
scene are

  * Backdrop: this could be a background
  * Tilemap(s): this could also be a background, or one or several tilemaps could
    complement a backdrop to build the background
  * Active BOBs and Sprites: The movable and animated objects that are drawn. The
    world subsystem will also perform collision detection on the active objects
  * Invisible objects: Those are only there for collision detection to create
    invisible walls or obstacles.

## Node hierarchy

Scenes have child nodes as a means to organize them into more manageable units.
At runtime, the world system recursively processes the scene tree to update all
the children. It is recommended to keep the tree fairly small, both for efficiency
and easier maintainability.
