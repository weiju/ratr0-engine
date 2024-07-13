# TODO list

## Roadmap

  * Tilesheet: rename palette to colors
  * copper list: make a copper list compiler and a file format
  * copper list should be part of the scene
  * sprite multiplexing
  * single plane interleaved blit to support lots of BOBs in Space invaders
  * grid-based movement
  * add a way to easily implement state pattern
    * have a "active" BOB list
    * if a BOB changes from active to inactive, add its covered rectangles to dirty
      That means adding and removal needs to be a part of "world" so we can track
      BOBs becoming active/inactive
  * tilemaps
  * add palette interpolation
  * look into making world system serializable / persistent

### Editor

  * Explore creating an editor for animations and tilesets with Electron

### Collision system

  * implement collision detection with quad tree
  * alternative: spatial hashing

### Scenes
  * State pattern support
  * object spawning / destruction
  * sub-pixel movement: this happens naturally by using procedural movement
  * add invisible objects that can have collision boxes (e.g. for invisible walls)
  * Hierarchical geometry: calculate parent-child positioning
    * how to unify that with quadtree-collision system
    * can it be done efficiently ????

### Rendering

  * implement frame animation for HW sprites
  * tilemaps
  * Palette interpolations
  * implement screen shake with scrolling technique 
  * research how to do sprite multiplexing with the copper
  * CONSIDER LATER
  * non-linear animation (using a function to flip frames)
  * 8x8 blitting
    * do font blitting with a color
    * BOB blitting with mask and shift (maybe the regular one just works, but test it !)
  *  Make display ad DMA fetch changeable
    * add variable display sizes, change DMA start stop and display window
    * sizes accordingly
  * Should active BOB, background, tilemaps and sprites be part of display ?
    For now, we can leave it in the scene
  * Known bugs:
    * fix bug: overlapped BOBs need to be redrawn (but not the rectangles), compute
      on the bounds and not on the collision boxes

#### Later

  * add Dual-playfield support
  * Copper list compiler/editor

### Memory


### Input 

  * Support Keyboard
  * Support mouse

### Other

  * integrate timer system
  * add timer events
  * integrate event system into world
  * Engine as library
  * mirroring background by setting negative modulos (a la Stardust)

### Games projects

  * Tetris
    * Sprites for player, preview, hold and score, Blitter for board
  * Space Invaders
    * Sprites for aliens and shots, blitter for the barricade
  * Centipede
    * sprites for score, player, special enemies and shot
    * BOBs for centipede, and board
  * Match 3
    * support mouse
    * sprites for score and effects
    * blitter for board
  * First person dungeon crawler
    * blit walls
    * support dialog
    * UI
