# TODO list

## Roadmap

  * Scene update function that is run every iteration
  * add a way to easily implement state pattern
    * have a "active" BOB list
    * if a BOB changes from active to inactive, add its covered rectangles to dirty
      That means adding and removal needs to be a part of "world" so we can track
      BOBs becoming active/inactive
  * integrate simple input to control movement
  * add palette interpolation
  * subpixel movement
  * integrate BOB movement with the rendering pipeline
    * fix bug: overlapped BOBs need to be redrawn (but not the rectangles), compute
      on the bounds and not on the collision boxes

1. 8x8 blitting
   * do font blitting with a color
   * BOB blitting with mask and shift (maybe the regular one just works, but test it !)
2. Make display ad DMA fetch changeable
   add variable display sizes, change DMA start stop and display window
   sizes accordingly

### Editor

  * Explore creating an editor for animations and tilesets with Electron

### Collision system

  * implement collision detection

### World

  * State pattern support
  * object spawning / destruction
  * object scripts
  * sub-pixel movement
  * add invisible objects that can have collision boxes (e.g. for invisible walls)

### Rendering

  * implement frame animation for HW sprites
  * tilemaps
  * Palette interpolations
  * implement screen shake with scrolling technique 
  * research how to do sprite multiplexing with the copper
  * non-linear animation (using a function to flip frames)

#### Later

  * add Dual-playfield support
  * Copper list compiler/editor

### Memory


### Input 

  * Support joystick

### Other

  * integrate timer system
  * add timer events
  * integrate event system into world
  * Engine as library

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
