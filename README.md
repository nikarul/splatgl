Splat 2D Rendering Library
====
Splat is a graphics library based on SDL 2.0 and OpenGL 2.0.  It provides an
efficient way to draw 2D sprites with basic effects such as rotation and
scaling.

Splat works by drawing on a canvas object.  A canvas contains an arbitrary
number of layers, each of which contain instances of images created from
SDL_Surfaces.  These instances can be positioned and changed as needed each
frame.  There can be multiple canvases, which can be rendered independently.

Splat provides a C/C++ and Python interface.  Splat is written in C++11,
however the interface is C-only to provide a clean ABI, although it is
perfectly usable in C++.  As Splat is currently in development, the ABI is not
stable, however once version 1.0, the plan is to guarantee a stable ABI within
major versions of the library.

This library is distributed under the zlib license, which can be found in the
file "COPYING".  It is the same license used by SDL 2.0.

Contributing
============



TODO
====
Finish README
Cleanup and generate docs
API Cleanup
Add API sanity checks
Make a logo
Upload to Github

Post publication
================
Multi-window support
Shader support

