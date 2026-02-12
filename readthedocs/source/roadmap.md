# Roadmap for RATR0

RATR0 in its current state is a C library that targets Amiga
classic OCS and ECS hardware.

It is implemented as a thin layer on top of Amiga hardware features
and focuses on facilitating the creation of arcade games in C.

Ease of use takes a backseat in favor of simplicity and making it
possible to allow for games that run at frame rates that match the
NTSC and PAL standards (60 and 50 frames per second).

There will be a lot to figure out on the way, and it is only the
initial stage. If this turns out to be successful, the next steps are

  * Allowing for declarative specification of the game
  * logic definition in a higher programming language
  * a port to modern systems that match the specifications of the
    Amiga, so we can port games by simply recompiling them for
    the modern platform
