# TODO list

## Roadmap

0. Change computation
  * we only need to blit things again when they move, animate or change
    * BOB frames
    * dirty rectangles

1. 8x8 blitting
   * do font blitting with a color
   * BOB blitting with mask and shift (maybe the regular one just works, but test it !)
2. Make display and DMA fetch changeable
   add variable display sizes, change DMA start stop and display window
   sizes accordingly

## Amiga

### Rendering

  * implement frame animation
  * tilemaps
  * implement screen shake with scrolling technique 
  * research how to do sprite multiplexing with the copper

#### Later

  * add Dual-playfield support
  * Copper list compiler/editor

### Memory

  * Allocate smaller blocks of memory at once in the allocator,
    AmigaOS can't reclaim large blocks very well

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
