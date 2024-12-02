# Centipede example

## Description

This is an example that uses a lot of sprite multiplexing.
All moving objects (player, shots, centipede, bugs) are sprites
to avoid slowdowns because of drawing

Centipede uses mostly blocks of 8x8, which is not great to process
for the Amiga blitter that prefers multiples of 16 pixels

## Technical implementation

Since the centipede can be split into multiple centipedes, we can have more
than one centipede object in the scene. Each Centipede object has 1-12 body
parts, with the first part being the head.

The idea is that instead of the centipede objects having a fixed number of
sprites, they instead will maintain a pool of sprites that they are sending
to the display system.

Since a body part is 8 pixel wide we can e.g. represent 2 body parts with
a single, 16 pixel sprite, hereby reducing the number of sprites we need.

That requires that the game will have to manage sprites itself.

### Sprite allocation

We will always allocate sprite 0 to the player sprite. We can actually
the shot sprite and the player using a single sprite, which would
free up sprite 1 for other purposes.
We need to, however, consider that sprite 0 and 1 are in the same sprite
group, and collisions between them are not automatically detected by
the hardware. So for now, player and shot will be represented by sprite 0 and
1 respectively.

We use sprites 2-7 to represent enemies.

We will have to spread the load among

  * 1-12 centipede segments
  * 1 spider
  * 1 flea
  * 1 scorpion

so up to 15 objects that have to be represented by 6 physical sprites
The spider, flea and scorpion only show up once in a while.
Technically, to most centipedes will have more than one body part.
so the worst cast would happen if 6 heads are alive.

So in general, a maximum of 6 sprites should be sufficient to represent the
centipede.

We could represent the rare enemies by BOBs, because the worst case would
be 3 enemies at once, which is no problem at all with the available compute
time.

