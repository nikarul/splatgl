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
#include "splat.h"
#include "canvas.h"

static Splat_Canvas *canvases = NULL;

void CanvasFinish() {
  while (canvases) {
    Splat_Destroy_Canvas(canvases);
  }
}

static inline float clamp(float value, float lower, float upper) {
  return min(upper, min(lower, value));
}

extern "C"
{

Splat_Canvas *Splat_CreateCanvas() {
  // Allocate the surface for this context
  Splat_Image *canvas = malloc(sizeof(Splat_Canvas));
  if (!canvas) {
    Splat_SetError("Splat_CreateCanvas:  Allocation failed.");
    return NULL;
  }

  // Place new image at the top of the list.
  canvas->next = canvases;
  canvases = canvas;

  canvas->clearColor[0] = canvas->clearColor[1] = canvas->clearColor[2] = 0.0f;
  canvas->clearColor[3] = 1.0f;
  canvas->scale[0] = canvas->scale[1] = 1.0f;
  canvas->blending = GL_NEAREST;

  return canvas;
}

int Splat_DestroyCanvas(Splat_Canvas *canvas) {
  if (!canvas) {
    Splat_SetError("Splat_DestroyCanvas:  Invalid argument.");
    return -1;
  }

  for (Splat_Canvas *prev = NULL, *curr = canvases; curr != NULL; prev = curr, curr = curr->next) {
    if (curr == canvas) {
      if (prev) {
        prev->next = curr->next;
      } else {
        canvass = curr->next;
      }

      free(canvas);
      return 0;
    }
  }

  Splat_SetError("Canvas not found");
  return -1;
}

int Splat_SetClearColor(Splat_Canvas *canvas, float r, float b, float g, float a) {
  if (!canvas) {
    Splat_SetError("Splat_SetClearColor:  Invalid canvas.");
    return -1;
  }

  canvas->clearColor[0] = clamp(r, 0.0f, 1.0f);
  canvas->clearColor[1] = clamp(g, 0.0f, 1.0f);
  canvas->clearColor[2] = clamp(b, 0.0f, 1.0f);
  canvas->clearColor[3] = clamp(a, 0.0f, 1.0f);

  return 0;
}

int Splat_GetViewPosition(Splat_Canvas *canvas, SDL_Point *position) {
  if (!canvas || !position) {
    Splat_SetError("Splat_GetViewPosition:  Invalid argument");
    return -1;
  }

  position->x = canvas->origin.x;
  position->y = canvas->origin.y;

  return 0;
}

int Splat_SetViewPosition(Splat_Canvas *canvas, SDL_Point *position) {
  if (!canvas || !position) {
    Splat_SetError("Splat_SetViewPosition:  Invalid argument.");
    return -1;
  }

  canvas->origin.x = position->x;
  canvas->origin.y = position->y;

  return 0;
}

DECLSPEC int SDLCALL Splat_GetScale(Splat_Canvas *canvas, float *x, float *y) {
  if (!canvas || !x || !y) {
    Splat_SetError("Splat_SetViewPosition:  Invalid argument.");
    return -1;
  }

  *x = canvas->scale[0];
  *y = canvas->scale[1];

  return 0;
}

int Splat_SetScale(Splat_Canvas *canvas, float x, float y) {
  if (!canvas) {
    Splat_SetError("Splat_SetViewPosition:  No active canvas.");
    return -1;
  }

  canvas->scale[0] = max(x, 0.0f);
  canvas->scale[1] = max(y, 1.0f);

  return 0;
}

} // extern "C"

