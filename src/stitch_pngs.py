#!/usr/bin/env python3

"""
Paste an indexed PNG image based on the first image
The first image is used as the reference, using its width and height
and palette.

The rest of the images are pasted into the first image.
The motivation is to assemble an image from strips that can
then have different colors that are set in the game's copper list
"""
from PIL import Image
import argparse
import os, math, sys

DESCRIPTION = """stitch_pngs - PNG file stitcher
"""


def get_colors(im):
    if im.palette is not None:
        palette_bytes = list(im.palette.palette) if im.palette.rawmode else im.palette.tobytes()
        colors = [i for i in chunks([b for b in palette_bytes], 3)]
        depth =  math.ceil(math.log(len(colors), 2))
    else:
        colors = [[0, 0, 0], [255, 255, 255]]
        depth = 1
    return depth, colors


def chunks(l, n):
    for i in range(0, len(list(l)), n):
        yield l[i:i+n]


def concat_images(images):
    images[0].paste(images[1], (0, 200))
    return images[0]


if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter,
                                     description=DESCRIPTION)

    parser.add_argument('pngfile', nargs='+', help="input PNG files")
    parser.add_argument('outfile', help="output PNG files")
    args = parser.parse_args()
    if os.path.exists(args.outfile):
        sys.exit("'%s' exists, please specify a new file" % args.outfile)

    ref_width = None
    ref_depth = None
    ref_palette = None
    in_images = []
    for f in args.pngfile:
        if not os.path.exists(f):
            sys.exit("File %s does not exist" % f)

        im = Image.open(f)
        if ref_width is None:
            ref_width = im.size[0]
        else:
            if im.size[0] != ref_width:
                sys.exit("All images need to have the same width")

        depth, colors = get_colors(im)

        if ref_depth is None:
            ref_depth = depth
        else:
            if depth != ref_depth:
                sys.exit("All images need to have the same depth")

        if ref_palette is None:
            ref_palette = im.palette
        in_images.append(im)

    # paste the images into the first image, which is the reference
    result = concat_images(in_images)
    result.save(args.outfile)
