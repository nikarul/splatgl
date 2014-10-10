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

extern "C" {

Splat_Layer *Splat_CreateLayer(Splat_Canvas *canvas) {
  if (!canvas) {
	Splat_SetError("Splat_CreateLayer:  Invalid argument.");
	return nullptr;
  }

  // ALlocate the surface for this context
  canvas->layers.emplace_back();
  Splat_Layer &layer(canvas->layers.back());

  //TODO init layer

  return &layer;
}

int Splat_DestroyLayer(Splat_Layer *layer) {
  if (!layer) {
	Splat_SetError("Splat_DestroyLayer:  Invalid argument.");
	return -1;
  }

  list<Splat_Layer> &layers = layer->canvas->layers;
  for (auto it = layers.begin(), end = layers.end(); it != end; ++it) {
	if (&(*it) == layer) {
	  layers.erase(it);
	  return 0;
	}
  }

  Splat_SetError("Splat_DestroyLayer:  Layer not found in canvas layers list.");
  return -1;
}

int Splat_MoveLayer(Splat_Layer *layer, Splat_Layer *other) {
  if (!layer || !other) {
	Splat_SetError("Splat_MoveLayer:  Invalid argument.");
	return -1;
  }

  if (layer->canvas != other->canvas) {
	Splat_SetError("Splat_MoveLayer:  Layers do not belong to the same canvas.");
	return -1;
  }

  Splat_Canvas *canvas = layer->canvas;
  auto itLeft = find(canvas->layers.begin(), canvas->layers.end(), *layer);
  if (itLeft == canvas->layers.end()) {
	Splat_SetError("Splat_MoveError:  Layer not found in active canvas.");
	return -1;
  }

  auto itRight = (other) ? find(canvas->layers.begin(), canvas->layers.end(), *other) : canvas->layers.begin();
  if (itRight == canvas->layers.end()) {
	Splat_SetError("Splat_MoveError:  'Other' layer not found in active canvas.");
	return -1;
  }

  swap(*itLeft, *itRight);
  return 0;
}

} // extern "C"

