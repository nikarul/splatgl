/*
  Splat Graphics Library
  Copyright (C) 2014  Michael Dale Long <mlong@digitalbytes.net>
  http://digitalbytes.net/projects/splatgl/

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef __SPLAT_CANVAS_H__
#define __SPLAT_CANVAS_H__

#include <SDL.h>
#include "types.h"

typedef struct Splat_Canvas {
  float clearColor[4];
  SDL_Point origin;
  float scale[2]; // Scale factors for X and Y
  Splat_Layer *layers;
  Splat_Rect *rects; // List of debug rects
  Splat_Line *lines; // List of debug lines
  struct Splat_Canvas *next;
} Splat_Canvas;

void CanvasFinish();

#endif // __SPLAT_CANVAS_H__

