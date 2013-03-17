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

#ifndef __LIB_SPLAT_SRC_DCANVAS_H__
#define __LIB_SPLAT_SRC_DCANVAS_H__

#include <GL/gl.h>
#include <forward_list>
#include "splat.h"
#include "dobject.h"
#include "dimage.h"
#include "dinstance.h"
#include "dlayer.h"

namespace Splat {

struct DebugLine {
  SDL_Point start;
  SDL_Point end;
  color_t color;
  int width;
  int32_t expire;
  bool relative;
};

struct DebugRect {
  SDL_Rect rect;
  color_t color;
  int32_t expire;
  bool filled;
  bool relative;
};

class SPLAT_LOCAL DCanvas : public DObject {
public:
  Canvas *q;
  SDL_Window *window;

  color_t clearColor;
  SDL_Point viewPos;
  scale_t scale; // Scale factors for X and Y
  std::forward_list<QImage> images;
  std::forward_list<DebugRect> rects;
  std::forward_list<DebugLine> lines;
  std::forward_list<QLayer> layers;

  std::array<float, 18> vertex_buffer;
  std::array<float, 12> texcoord_buffer;

  DCanvas(Canvas *canvas, SDL_Window *window);
  ~DCanvas();

  SPLAT_INLINE Image *CreateImage(SDL_Surface *surface);
  SPLAT_INLINE void DestroyImage(Image *image);

  SPLAT_INLINE Layer *CreateLayer();
  SPLAT_INLINE void DestroyLayer(Layer *layer);

  SPLAT_INLINE void SetClearColor(const color_t &color);

  SPLAT_INLINE const SDL_Point &GetViewPosition() const { return viewPos; }
  SPLAT_INLINE void SetViewPosition(int x, int y);

  SPLAT_INLINE const scale_t &GetViewScale() const { return scale; }
  SPLAT_INLINE void SetViewScale(float x, float y);

  void Render();

  void DrawRect(SDL_Rect *rect, const color_t &color, int width, int ttl, bool filled, bool relative);
  void DrawLine(SDL_Point *start, SDL_Point *end, const color_t &color, int width, int ttl, bool relative);
};

Image *DCanvas::CreateImage(SDL_Surface *surface) {
  images.emplace_front(surface);
  return &images.front();
}

Layer *DCanvas::CreateLayer() {
  layers.emplace_front(q);
  return &layers.front();
}

void DCanvas::DestroyLayer(Layer *layer) {
  if (layer == nullptr) {
    throw BadParameterException();
  }

  if (layer->d->canvas != q) {
    throw WrongCanvasException();
  }

  layers.remove_if([=](const Layer &other) { return other.d == layer->d; });
}

void DCanvas::SetClearColor(const color_t &color) {
  glClearColor(color[0], color[1], color[2], color[3]);
}

void DCanvas::SetViewPosition(int x, int y) {
  viewPos.x = x;
  viewPos.y = y;
}

void DCanvas::SetViewScale(float x, float y) {
  scale[0] = x;
  scale[1] = y;
}

}

#endif // __LIB_SPLAT_SRC_CANVAS_H__

