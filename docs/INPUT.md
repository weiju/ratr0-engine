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

  * Rather than responding to specific inputs, the input system instead
    defines actions, and input types can be mapped to actions. With this
    not only can we remap inputs, but also map multiple input types to
    one action
  * Actions are represented with an ID that is allocated on demand at
    engine configuration time.
  * The input system is updated once per frame and synchronized to the video beam
    to ensure stable low latency, that means for Amiga that the input system is
    updated in the vertical blank interrupt.
