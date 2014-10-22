Splat Graphics Library
====

Splat is a graphics library based on SDL 2.0 and OpenGL 2.0.  It aims to
provide an efficient way to draw 2D sprites with basic effects such as
rotation and scaling.

Splat works by drawing on a canvas object.  A canvas contains an arbitrary
number of layers, each of which contain instances of images created from
SDL_Surfaces.  These instances can be positioned and changed as needed each
frame.  There can be multiple canvases, which can be rendered independently.

This library is distributed under the zlib license, which can be found in
the file "COPYING".

Contributing
============

The most straightforward way to start contributing is to use it in a project
and provide feedback/bug reports. Patches and pull requests for bug fixes and
new features are welcome.

Splat is still under development, and it's API is still evolving.  Some plans
for the future include:

* Allowing Canvases to be attached to different SDL_Windows, by moving much
of the Splat_Prepare() logic to Splat_CreateCanvas().

* Support for transitioning between canvases and renderering multiple
canvases on the same window.

* Adding support for using shaders with Splat images.  Primarily for
supporting effects with fragment shaders, but allowing other types of shaders
as well.

* Support for static images, images which are added to a canvas and cannot
be changed or removed for the lifetime of the canvas.

* Better validation of arugments public functions.

* Finishing Doxygen comments in splat.h and generating documentation from
them.

* Proper memory clean up, don't rely on the calling application to clean up
all open handles prior to calling Splat_Finish().

* Porting to more SDL/OpenGL supported platforms, including OpenGL ES
platforms.

* Creation of a sample application or application suite, probably in a
separate git repository.

* Adding more language bindings.

* Stabilizing the API and ABI for a 1.0 release.

* Review the use of GL_GLEXT_PROTOYPES for OpenGL extension functions and
determine if it would be better to dynamically load the functions and disable
them if not available.

