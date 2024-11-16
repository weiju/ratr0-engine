#!/usr/bin/env python3

"""
A fader utility that takes a tiles file and creates a fader data
structure.
We can specify a start color (default black) that can be overridden,
the palette entry number and the number of steps
For now, interpolation is linear
"""

import argparse
import os

DESCRIPTION = """ratr0-fader - generate fader values
"""

if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter,
                                     description=DESCRIPTION)
    parser.add_argument("infile", help="input file")
    parser.add_argument("outfile", help="output file")
    parser.add_argument("--startcolor", type=int, default=0xfff)
    parser.add_argument("--numsteps", type=int, default=50)
    args = parser.parse_args()
