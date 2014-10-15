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

#include <assert.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "splat.h"
#include "types.h"
#include "canvas.h"

Splat_Layer *Splat_CreateLayer(Splat_Canvas *canvas) {
  if (!canvas) {
    Splat_SetError("Splat_CreateLayer:  Invalid argument.");
    return NULL;
  }

  // Allocate the layer
  Splat_Layer *layer = malloc(sizeof(Splat_Layer));
  if (!layer) {
    Splat_SetError("Splat_CreateLayer:  Allocation failed.");
    return NULL;
  }

  layer->canvas = canvas;
  layer->instances = NULL;
  layer->next = NULL;

  // Place new layer at the bottom of the list.
  if (canvas->layers) {
    Splat_Layer *parent = canvas->layers;
    while (parent->next) {
      parent = parent->next;
    }
    parent->next = layer;
  }
  else {
    canvas->layers = layer;
  }

  return layer;
}

int Splat_DestroyLayer(Splat_Layer *layer) {
  if (!layer) {
    Splat_SetError("Splat_DestroyLayer:  Invalid argument.");
    return -1;
  }

  for (Splat_Layer *prev = NULL, *curr = layer->canvas->layers; curr != NULL; prev = curr, curr = curr->next) {
    if (curr == layer) {
      if (prev) {
        prev->next = curr->next;
      } else {
        layer->canvas->layers = curr->next;
      }

      free(layer);
      return 0;
    }
  }

  Splat_SetError("Splat_DestroyLayer:  Layer not found in canvas layers list.");
  return -1;
}

int Splat_MoveLayer(Splat_Layer *layer, Splat_Layer *other) {
  if (!layer || !other || layer == other) {
    Splat_SetError("Splat_MoveLayer:  Invalid argument.");
    return -1;
  }

  if (layer->canvas != other->canvas) {
    Splat_SetError("Splat_MoveLayer:  Layers do not belong to the same canvas.");
    return -1;
  }

  Splat_Layer *layerPrev = NULL, *otherPrev = NULL;
  int found = 0;

  for (Splat_Layer *prev = NULL, *curr = layer->canvas->layers; found < 4 && curr != NULL; prev = curr, curr = curr->next) {
    if (curr == layer) {
      layerPrev = prev;
      assert((found & 1) == 0);
      found |= 1;
    }

    if (curr == other) {
      otherPrev = prev;
      assert((found & 2) == 0);
      found |= 2;
    }
  }

  if (found != 3) {
    Splat_SetError("Splat_MoveLayer:  One or both layers were not found in the canvas");
    return -1;
  }

  // Swap the next pointers for the two layers
  Splat_Layer *tmp = layer->next;
  layer->next = other->next;
  other->next = tmp;

  if (layerPrev) {
    layerPrev->next = other;
  } else {
    layer->canvas->layers = other;
  }

  if (otherPrev) {
    otherPrev->next = layer;
  } else {
    layer->canvas->layers = layer;
  }

  return 0;
}

