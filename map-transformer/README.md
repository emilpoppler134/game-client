# map-transformer

## What is this doing here?

This is a (hopefully) small script with the purpose to transform a .bmp file containing the entire map of the game, to a number of smaller files of a custom format, which is then read by the game.

## How it works

### Read root map file

Reads from: `/src/assets/map/root-map.bmp`. Only certain color values are permitted.

### Convert color to index

Currently converts 24-bit color to a 1 byte per pixel format through a lookup table `map`.

### Do auto tiling stuff

Depending on if the neighboors tiles are land or water, a different type of land tile is chosen at the center.

### Write to custom files

Divides the image and writes byte values to 256 files named 00.mapchunk to FF.mapchunk.
