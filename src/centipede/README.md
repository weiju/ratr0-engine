# Centipede example

## Description

This is an example that uses a lot of sprite multiplexing.
All moving objects (player, shots, centipede, bugs) are sprites
to avoid slowdowns because of drawing

Centipede uses mostly blocks of 8x8, which is not great to process
for the Amiga blitter that prefers multiples of 16 pixels
