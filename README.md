VELES
=====

This is a modern re-implementation of the old
[Veles visualization](https://codisec.com/binary-visualization-explained/).
Unlike the original implementation proposed by codisec, this implementation
pre-processes the data into a 3D voxel space which is then rendered via 3D
textures. As a result, rendering performance is completely independent of the
data size and even large input files can be visualized without problems.


System Requirements
-------------------

- Linux (may or may not work on other OS to, but only Linux is tested for now)
- gcc, make
- OpenGL 3.3+
- freeglut
- glslang
- [bin2o](https://github.com/hackyourlife/bin2o)


Building
--------

Just run `make` and you will get a `veles` binary that you can run.


Usage
-----

Start the visualization with `veles the-file`. This will open a window with the
visualization. Use the mouse (left button) to rotate the visualization.
Using the right mouse button, you can adjust the brightness (vertical movement)
as well as the contrast (horizontal movement).

In addition, the following keys invoke an action:

- `a`: Accumulate pixels instead of blending
- `b`: Use alpha blending
- `c`: Change color palette
- `g`: Use standard brightness
- `l`: Use extra low brightness (useful if your plot is way too bright)
- `p`: Use nearest neighbor interpolation for intensity ("pixel look")
- `s`: Use linear interpolation for intensity ("smooth look")
- `n`: Use nearest neighbor interpolation for position ("correct colors")
- `f`: Use linear interpolation for position ("wrong colors")
- `r`: Reset rotation, brightness, contrast and palette


Why?
----

Someone said "I tried to load a game ISO into Veles and it just crashed", so I
thought "why is file size even a problem anyway?" and as it turns out, it is
not a problem for a proper implementation.


TODOs
-----

Right now an orthographic projection is used, but this is quite a mindfuck for
the "pixel cloud" type visualization. Implementing a perspective projection
might be more useful.

There is also regularly the question of "I see this strange pattern in the
plot, but where does it come from?", but answering this question would require
adding some mechanism to attribute individual points to locations in the source
file.


Screenshots
-----------

GBA ROM:

![GBA ROM](https://raw.githubusercontent.com/hackyourlife/veles/master/doc/gba-rom.png)

PDP-11 (RL02) disk image:

![RL02 disk](https://raw.githubusercontent.com/hackyourlife/veles/master/doc/pdp11-disk.png)

MP4 file:

![MP4 file](https://raw.githubusercontent.com/hackyourlife/veles/master/doc/mp4-file.png)

FLAC file:

![FLAC file](https://raw.githubusercontent.com/hackyourlife/veles/master/doc/flac-file.png)
