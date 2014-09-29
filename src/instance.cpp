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

Splat_Instance *Splat_CreateInstance(Splat_Image *image, Splat_Layer *layer, int x, int y, float s1, float t1, float s2, float t2, uint32_t flags) {
  if (!layer) {
	Splat_SetError("Splat_CreateInstance called with NULL layer");
	return nullptr;
  }

  // Allocate the surface for this context
  layer->instances.emplace_front();
  Splat_Instance &instance(layer->instances.front());

  // Setup the handle
  instance.texture = image->texture;
  instance.rect.x = x;
  instance.rect.y = y;
  instance.rect.w = image->width * (s2 - s1);
  instance.rect.h = image->height * (t2 - t1);

  instance.s1 = s1;
  instance.t1 = t1;
  instance.s2 = s2;
  instance.t2 = t2;
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
  forward_list<Splat_Instance> &instances = instance->layer->instances;
  for (auto prev = instances.before_begin(), it = instances.begin(), end = instances.end(); it != end; prev = it, ++it) {
	if (&(*it) == instance) {
	  instances.erase_after(prev);
	  return 0;
	}
  }

  Splat_SetError("Instance not found");
  return -1;
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

int Splat_SetInstanceImage(Splat_Instance *instance, Splat_Image *image, float s1, float t1, float s2, float t2) {
  instance->texture = image->texture;
  instance->s1 = s1;
  instance->t1 = t1;
  instance->s1 = s1;
  instance->s1 = s1;

  return 0;
}

} // extern "C"

