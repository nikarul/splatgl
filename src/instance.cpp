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

#include <SDL.h>
#include <SDL_opengl.h>
#include "splat.h"
#include "types.h"
#include "canvas.h"

extern "C"
{

Splat_Instance *Splat_CreateInstance(Splat_Image *image, Splat_Layer *layer, int x, int y, float s1, float t1, float s2, float t2, uint32_t flags) {
  if (!layer) {
	Splat_SetError("Splat_CreateInstance:  Invalid argument");
	return NULL;
  }

  // Allocate the instance
  Splat_Instance *instance = malloc(sizeof(Splat_Instance));
  if (!instance) {
	Splat_SetError("Splat_CreateInstance:  Allocation failed.");
	return NULL;
  }

  // Place new instance at the top of the list.
  instance->next = layer->instances;
  layer->instances = instance;

  // Setup the handle
  instance->texture = image->texture;
  instance->rect.x = x;
  instance->rect.y = y;
  instance->rect.w = image->width * (s2 - s1);
  instance->rect.h = image->height * (t2 - t1);

  instance->s1 = s1;
  instance->t1 = t1;
  instance->s2 = s2;
  instance->t2 = t2;
  instance->layer = layer;
  instance->scale[0] = 1.0f;
  instance->scale[1] = 1.0f;
  instance->angle = 0.0f;
  instance->color.r = instance->color.g = instance->color.b = instance->color.a = 255;
  instance->clip.x = instance->clip.y = instance->clip.w = instance->clip.h = 0;
  instance->flags = flags;
  instance->nextCulledHandle = NULL;

  return instance;
}

int Splat_DestroyInstance(Splat_Instance *instance) {
  if (!instance) {
    Splat_SetError("Splat_DestroyInstance:  Invalid argument.");
    return -1;
  }

  Splat_Layer *layer = instance->layer;
  for (Splat_Instance *prev = NULL, *curr = layer->instances; curr != NULL; prev = curr, curr = curr->next) {
    if (curr == instance) {
      if (prev) {
        prev->next = curr->next;
      } else {
        layer->instances = curr->next;
      }

      free(instance);
      return 0;
    }
  }

  Splat_SetError("Instance not found");
  return -1;
}

int Splat_SetInstancePosition(Splat_Instance *instance, int x, int y) {
  if (!instance) {
    Splat_SetError("Splat_SetInstancePosition:  Invalid argument.");
    return -1;
  }

  instance->rect.x = x;
  instance->rect.y = y;
  return 0;
}

int Splat_SetInstanceLayer(Splat_Instance *instance, Splat_Layer *layer) {
  if (!instance || !layer) {
    Splat_SetError("Splat_SetInstanceLayer:  Invalid argument.");
    return -1;
  }

  // Don't do anything if we're trying to move
  if (layer == instance->layer) {
    return 0;
  }

  Splat_Layer *oldlayer = instance->layer;
  for (Splat_Instance *prev = NULL, *curr = oldlayer->instances; curr != NULL; prev = curr, curr = curr->next) {
    if (curr == instance) {
      // Remove instance from old layer
      if (prev) {
        prev->next = curr->next;
      } else {
        oldlayer->instances = curr->next;
      }

      // Add instance to new layer
      instance->next = layer->instances;
      layer->instances = instance;
      return 0;
    }
  }

  Splat_SetError("Splat_SetInstanceLayer:  Instance not found in current layer.");
  return -1;
}

int Splat_SetInstanceImage(Splat_Instance *instance, Splat_Image *image, float s1, float t1, float s2, float t2) {
  if (!instance || !image) {
    Splat_SetError("Splat_SetInstanceLayer:  Invalid argument.");
    return -1;
  }

  instance->texture = image->texture;
  instance->s1 = s1;
  instance->t1 = t1;
  instance->s1 = s1;
  instance->s1 = s1;

  return 0;
}

} // extern "C"

