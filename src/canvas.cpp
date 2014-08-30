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

#include <SDL_opengl.h>
#include <algorithm>
#include "splat.h"
#include "canvas.h"

static vector<Splat_Canvas> canvases;
Splat_Canvas *activeCanvas = nullptr;

void CanvasFinish() {
  canvases.clear();
  activeCanvas = nullptr;
}

extern "C"
{

Splat_Canvas *Splat_CreateCanvas() {
  canvases.emplace_back();
  Splat_Canvas &canvas(canvases.back());

  canvas.clearColor[0] = canvas.clearColor[1] = canvas.clearColor[2] = 0;
  canvas.clearColor[3] = 1.0f;
  canvas.scale[0] = canvas.scale[1] = 1.0f;
  canvas.blending = GL_NEAREST;

  return &canvas;
}

int Splat_DestroyCanvas(Splat_Canvas *canvas) {
  if (!canvas) {
    Splat_SetError("Splat_DestroyCanvas:  NULL canvas pointer.");
    return -1;
  }

  //remove(canvases.begin(), canvases.end(), *canvas);
  return 0;
}

Splat_Canvas *Splat_GetActiveCanvas() {
	return activeCanvas;
}

void Splat_SetActiveCanvas(Splat_Canvas *canvas) {
	activeCanvas = canvas;
}

} // extern "C"

