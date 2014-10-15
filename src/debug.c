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

#include "splat.h"
#include "types.h"
#include "canvas.h"

int Splat_DrawRect(Splat_Canvas *canvas, SDL_Rect *rect, SDL_Color *color, int width, int flags, int ttl) {
  if (!canvas || !rect || !color) {
    Splat_SetError("Invalid argument");
    return -1;
  }

  // Allocate the rect
  Splat_Rect *r = malloc(sizeof(Splat_Rect));
  if (!r) {
    Splat_SetError("Splat_DrawRect:  Allocation failed.");
    return -1;
  }

  // Place new rect at the top of the list.
  r->next = canvas->rects;
  canvas->rects = r;

  r->x1 = rect->x;
  r->x2 = rect->x + rect->w;
  r->y1 = rect->y;
  r->y2 = rect->y + rect->h;

  r->width = width;
  memcpy(&r->color, color, sizeof(SDL_Color));
  r->ttl = ttl;
  r->relative = ((flags & SPLAT_RELATIVE) != 0);
  r->fill = ((flags & SPLAT_FILLED) != 0);

  return 0;
}

int Splat_DrawLine(Splat_Canvas *canvas, SDL_Point *start, SDL_Point *end, SDL_Color *color, int width, int flags, int ttl) {
  if (!canvas || !start || !end || !color) {
    Splat_SetError("Invalid argument");
    return -1;
  }

  // Allocate the line
  Splat_Line *line = malloc(sizeof(Splat_Line));
  if (!line) {
    Splat_SetError("Splat_DrawLine:  Allocation failed.");
    return -1;
  }

  // Place new line at the top of the list.
  line->next = canvas->lines;
  canvas->lines = line;

  memcpy(&line->start, start, sizeof(SDL_Point));
  memcpy(&line->end, end, sizeof(SDL_Point));
  memcpy(&line->color, color, sizeof(SDL_Color));
  line->width = width;
  line->ttl = ttl;
  line->relative = ((flags & SPLAT_RELATIVE) != 0);

  return 0;
}

