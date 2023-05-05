# RATR0 SDL based engine components

## Description

This package implements the SDL version of RATR0. We are actually
using SDL2.

### RATR0 image file formats

Since tilesets are stored in the RATR0TIL format, there need to be
specific loading functions to convert the image data into `SDL_Surface`
objects.

### Sprites

Since SDL is only a means to support modern architectures, we don't need
hardware sprites, instead, we will just use blitter objects all the time.
Instead of cookie cuts, we use alpha blending, so the tile set files
should be prepared as

  * non-interleaved
  * color 0 is always transparent
  * no mask
