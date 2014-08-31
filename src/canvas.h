/*
  Splat 2D Rendering Library
  Copyright (C) 2014  Michael Dale Long <mlong@digitalbytes.net>

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

#include <string>
#include <vector>
#include <SDL.h>
#include "types.h"

using namespace std;

struct Splat_Canvas {
  bool operator==(const Splat_Canvas &other) { return this == &other; }

  float clearColor[4];
  SDL_Point origin;
  float scale[2]; // Scale factors for X and Y
  vector<Splat_Image> images;
  vector<Splat_Layer> layers;
  vector<SDL_Rect> rects; // List of rects
  vector<Splat_Line> lines; // Python list of lines
  int blending;
};

extern Splat_Canvas *activeCanvas;

void CanvasFinish();

#endif // __SPLAT_CANVAS_H__

