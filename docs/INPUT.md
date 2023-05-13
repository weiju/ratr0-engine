# Input system

## Description

The RATR0 input system decouples system-specific inputs and maps
them into user-defined actions.
By this, the engine can support customizable inputs that can
be flexibly configured.

## Design ideas

The input system is inspired by other game engines like Godot, but
rather using string as identifiers, it uses integers to avoid
costly comparison or mapping functions.

  * Actions are represented with an ID that is allocated on demand at
    engine configuration time.
  * Actions are globally for a game, i.e. they apply to every scene in the game
  * The input system is updated once per frame and synchronized to the video beam
    to ensure stable low latency, that means for Amiga that the input system is
    updated in the vertical blank interrupt.
