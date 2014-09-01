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

#include <algorithm>
#include <SDL.h>
#include <SDL_opengl.h>
#include "splat.h"
#include "types.h"
#include "canvas.h"

extern "C"
{

Splat_Instance *Splat_CreateInstance(Splat_Image *image, Splat_Layer *layer, int x, int y, SDL_Rect *subimage, uint32_t flags) {
  // ALlocate the surface for this context
  layer->instances.emplace_back();
  Splat_Instance &instance(layer->instances.back());

  // Setup the handle
  instance.texture = image->texture;
  instance.rect.x = x;
  instance.rect.y = y;
  if (subimage) {
	// Sanitize input subrect
	subimage->x = max(subimage->x, 0);
	subimage->y = max(subimage->y, 0);
	subimage->w = min<int>(subimage->w, image->width);
	subimage->h = min<int>(subimage->h, image->height);

	instance.rect.w = subimage->w;
	instance.rect.h = subimage->h;

	instance.s1 = subimage->x / image->width;
	instance.t1 = subimage->y / image->height;
	instance.s2 = (subimage->x + subimage->w) / image->width;
	instance.t2 = (subimage->y + subimage->h) / image->height;
  } else {
	instance.rect.w = image->width;
	instance.rect.h = image->height;

	instance.s1 = 0.0f;
	instance.t1 = 0.0f;
	instance.s2 = 1.0f;
	instance.t2 = 1.0f;
  }
  instance.layer = layer;
  instance.scale[0] = 1.0f;
  instance.scale[1] = 1.0f;
  instance.angle = 0.0f;
  instance.color.r = instance.color.g = instance.color.b = instance.color.a = 255;
  instance.clip.x = instance.clip.y = instance.clip.w = instance.clip.h = 0;
  instance.flags = flags;
  instance.nextCulledHandle = nullptr;

  return &instance;
}

int Splat_DestroyInstance(Splat_Instance *instance) {
  remove(instance->layer->instances.begin(), instance->layer->instances.end(), *instance);
  return 0;
}

int Splat_SetInstancePosition(Splat_Instance *instance, int x, int y) {
  instance->rect.x = x;
  instance->rect.y = y;
  return 0;
}

int Splat_SetInstanceLayer(Splat_Instance *instance, Splat_Layer *layer) {
  Splat_SetError("TODO:  Implement Splat_SetInstanceLayer");
  return -1;
}

int Splat_SetInstanceImage(Splat_Instance *instance, Splat_Image *image, SDL_Rect *subimage) {
  instance->texture = image->texture;

  instance->rect.x = subimage->x;
  instance->rect.y = subimage->y;
  instance->rect.w = subimage->w;
  instance->rect.h = subimage->h;

  return 0;
}

} // extern "C"

