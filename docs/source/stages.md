# Stages system

## Description

The stages system is designed around stage objects. Stage object are containers
that can be automatically inspected and run by the Stages subsystem.
Only one stage can be active at any given moment, and it could represent a
level, or a user interface, like e.g. a title screen.

## Stages

A stage is a container for game objects. The stages system inspects the active
stage in every iteration of the game loop. The most important objects in the
stage are

  * Backdrop: a backdrop serves as an image buffer that is automatically
    used to restore rectangles that were obscured by BOBs. Very simple if
    you have single screen setups.
  * Tilemap(s): this could also be a background, or one or several tilemaps could
    complement a backdrop to build the background
  * Active BOBs and Sprites: The movable and animated objects that are drawn. The
    stages subsystem will also perform collision detection on the active objects
  * Invisible objects: Those are only there for collision detection to create
    invisible walls or obstacles.
