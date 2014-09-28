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

static forward_list<Splat_Canvas> canvases;
Splat_Canvas *activeCanvas = nullptr;

void CanvasFinish() {
  canvases.clear();
  activeCanvas = nullptr;
}

extern "C"
{

Splat_Canvas *Splat_CreateCanvas() {
  canvases.emplace_front();
  Splat_Canvas &canvas(canvases.front());

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

  for (auto prev = canvases.before_begin(), it = canvases.begin(), end = canvases.end(); it != end; prev = it, ++it) {
	if (&(*it) == canvas) {
	  canvases.erase_after(prev);
	  return 0;
	}
  }

  Splat_SetError("Canvas not found");
  return -1;
}

Splat_Canvas *Splat_GetActiveCanvas() {
	return activeCanvas;
}

void Splat_SetActiveCanvas(Splat_Canvas *canvas) {
	activeCanvas = canvas;
}

int Splat_SetClearColor(float r, float b, float g, float a) {
  if (!activeCanvas) {
	Splat_SetError("Splat_SetClearColor:  No active canvas.");
	return -1;
  }

  activeCanvas->clearColor[0] = r;
  activeCanvas->clearColor[1] = g;
  activeCanvas->clearColor[2] = b;
  activeCanvas->clearColor[3] = a;

  return 0;
}

int Splat_GetViewPosition(SDL_Point *position) {
  if (!position) {
	Splat_SetError("Splat_GetViewPosition:  Invalid argument.");
	return -1;
  }

  if (!activeCanvas) {
	Splat_SetError("Splat_GetViewPosition:  No active canvas.");
	return -1;
  }

  position->x = activeCanvas->origin.x;
  position->y = activeCanvas->origin.y;

  return 0;
}

int Splat_SetViewPosition(SDL_Point *position) {
  if (!position) {
	Splat_SetError("Splat_SetViewPosition:  Invalid argument.");
	return -1;
  }

  if (!activeCanvas) {
	Splat_SetError("Splat_SetViewPosition:  No active canvas.");
	return -1;
  }

  activeCanvas->origin.x = position->x;
  activeCanvas->origin.y = position->y;

  return 0;
}

DECLSPEC int SDLCALL Splat_GetScale(float *x, float *y) {
  if (!x || !y) {
	Splat_SetError("Splat_SetViewPosition:  Invalid argument.");
	return -1;
  }

  if (!activeCanvas) {
	Splat_SetError("Splat_SetViewPosition:  No active canvas.");
	return -1;
  }

  *x = activeCanvas->scale[0];
  *y = activeCanvas->scale[1];

  return 0;
}

int Splat_SetScale(float x, float y) {
  if (!activeCanvas) {
	Splat_SetError("Splat_SetViewPosition:  No active canvas.");
	return -1;
  }

  activeCanvas->scale[0] = x;
  activeCanvas->scale[1] = y;

  return 0;
}

} // extern "C"

