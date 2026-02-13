# Sprites

Sprites are moving and animated objects that are displayed on the screen.
On the Amiga, there are 2 possible ways to display sprites:

  * Hardware sprites: independent of playfield hardware
  * BOBs: these are images that are copied into the display buffer

## Hardware sprites

These are limited in terms of size and colors. However, they are vastly
superior to BOBs in terms of performance because they virtually don't
require any additional effort from the CPU or Blitter.

Hardware sprite data is stored different from BOB data - while the frames
are still layed out as a column, the image is always 16 pixels wide and
2 frames are separated through a pair of 16 bit words.

## Soft sprites (BOBs)

Flexible in size, color depth only limited by the playfield hardware.
Displaying a BOB can result in performance penalties.

BOB animation are stored as tile sets that are layed out in a specific format.
To keep things simple and consistent, animation frames are stored in a single
column, and the right side is padded so there are at least 16 pixels of space
at the right side.
Each frame is aligned to the left side and the image width is a multiple of
16 pixels. This format is in accordance to the operation of the Amiga Blitter.

## Creating a sprite in the RATR0 engine

Sprites can be created from a tilesheet


