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

Splat_Layer *Splat_CreateLayer() {
  // Ensure we have a valid canvas
  if (!activeCanvas) {
	Splat_SetError("No active canvas");
	return nullptr;
  }

  // ALlocate the surface for this context
  activeCanvas->layers.emplace_front();
  Splat_Layer &layer(activeCanvas->layers.front());

  //TODO init layer

  return &layer;
}

int Splat_DestroyLayer(Splat_Layer *layer) {
  // Ensure we have a valid canvas
  if (!activeCanvas) {
	Splat_SetError("No active canvas");
	return -1;
  }

  forward_list<Splat_Layer> &layers = activeCanvas->layers;
  for (auto prev = layers.before_begin(), it = layers.begin(), end = layers.end(); it != end; prev = it, ++it) {
	if (&(*it) == layer) {
	  layers.erase_after(prev);
	  return 0;
	}
  }

  Splat_SetError("Layer not found");
  return -1;
}

int Splat_MoveLayer(Splat_Layer *layer, Splat_Layer *other) {
  // Ensure we have a valid canvas
  if (!activeCanvas) {
	Splat_SetError("No active canvas");
	return -1;
  }

  if (activeCanvas->layers.empty()) {
	Splat_SetError("No layers in active canvas");
	return -1;
  }

  auto itLeft = find(activeCanvas->layers.begin(), activeCanvas->layers.end(), *layer);
  if (itLeft == activeCanvas->layers.end()) {
	Splat_SetError("Splat_MoveError:  Layer not found in active canvas.");
	return -1;
  }

  auto itRight = (other) ? find(activeCanvas->layers.begin(), activeCanvas->layers.end(), *other) : activeCanvas->layers.begin();
  if (itRight == activeCanvas->layers.end()) {
	Splat_SetError("Splat_MoveError:  'Other' layer not found in active canvas.");
	return -1;
  }

  swap(*itLeft, *itRight);
  return 0;
}

} // extern "C"

