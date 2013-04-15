/*
  Splat 2D Rendering Library
  Copyright (C) 2003-2013  Michael Dale Long <mlong@digitalbytes.net>

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
#include "dcanvas.h"

namespace Splat {

Canvas::Canvas(SDL_Window *window) : d(this, window) {}
Canvas::~Canvas() {}

Image *Canvas::CreateImage(SDL_Surface *surface) {
  return d->CreateImage(surface);
}

void Canvas::DestroyImage(Image *image) {
  d->DestroyImage(image);
}

void Canvas::SetClearColor(color_t& color) {
  d->SetClearColor(color);
}

Layer *Canvas::CreateLayer() {
  return d->CreateLayer();
}

Layer *Canvas::CreateLayer(Layer *upper) {
  return d->CreateLayer(upper);
}

void Canvas::MoveLayerToTop(Layer *layer) {
  d->MoveLayerToTop(layer);
}

void Canvas::MoveLayer(Layer *layer, Layer *upper) {
  d->MoveLayer(layer, upper);
}

void Canvas::DestroyLayer(Layer *layer) {
  d->DestroyLayer(layer);
}

void Canvas::Render() {
  d->Render();
}

void Canvas::DrawDebugRect(SDL_Rect *rect, color_t &color, unsigned int width, unsigned int ttl, bool filled, bool relative) {
  d->DrawRect(rect, color, width, ttl, filled, relative);
}

void Canvas::DrawDebugLine(SDL_Point *start, SDL_Point *end, color_t &color, unsigned int width, unsigned int ttl, bool relative) {
  d->DrawLine(start, end, color, width, ttl, relative);
}

}

