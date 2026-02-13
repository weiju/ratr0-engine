# Welcome to ratr0-utils

ratr0_utils is a collection of utilities for converting common multimedia formats
into the data formats that are supported by the RATR0 game engine, the Spartan
programming language (both yet unreleased) and the Youtube programming series
`"Amiga Hardware Programming in C" <https://www.youtube.com/playlist?list=PLF3LSwdkRB1DLYvsFJynIBnmlIXkuTwTZ>`_.

All of the above primarily target retro platforms (currently only the Commodore
Amiga is supported) and therefore typically utilizes planar graphics and 8 bit sound formats.

The general idea is to create data formats that need little or no processing
on the Amiga system and do all of the conversion offline on a more modern
systems where processing power is cheap and we have scripting languages
that greatly simplify writing tools.

## Installation

*Python 3 PIP*


The official and easiest way is to install through pip. Only Python 3 is supported,
so depending on whether your default Python installation is 2.x or Python 3,
the below ``pip`` invocation becomes ``pip3``:

```bash
pip install ratr0-utils
```

*From source*

Alternatively, it is possible to install the package from source

```bash
python setup.py install
```


## Command Line Tools
```{toctree}
:maxdepth: 1

commands/ratr0_maketiles
commands/ratr0_makesprites
commands/ratr0_converttiled
commands/ratr0_makecoplist
```

## Asset Formats

```{toctree}
:maxdepth: 1

formats/tile_format
formats/level_format
formats/sprite_format
```