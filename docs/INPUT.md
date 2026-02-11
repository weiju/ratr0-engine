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

### Keyboard input

Keyboard input is performed through polling the keyboard.device in each
iteration of the game loop. The status of the keyboard is obtained in
a bit array called the keyboard matrix.
The disadvantage is that only the raw keycode is available, and we would lose
all the mapping information the user has provided to the operating system.
Mapping is therefore performed by obtaining the current system keymap and
obtaining the logical key through a lookup in the keymap, possibly
incorporating one or more modifiers (shift, control, alt, etc.)
