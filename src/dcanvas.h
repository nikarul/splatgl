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
#include <vector>
#include <forward_list>
#include <memory>
#include "splat.h"
#include "dobject.h"
#include "dimage.h"
#include "dinstance.h"
#include "dlayer.h"

namespace Splat {

struct SPLAT_LOCAL DebugLine {
  SDL_Point start;
  SDL_Point end;
  color_t color;
  uint32_t width;
  uint32_t expire;
  bool relative;
};

struct SPLAT_LOCAL DebugRect {
  SDL_Rect rect;
  color_t color;
  uint32_t width;
  uint32_t expire;
  bool filled;
  bool relative;
};

class SPLAT_LOCAL DCanvas : public DObject {
public:
  Canvas *q;
  SDL_Window *window;
  SDL_GLContext glcontext;

  color_t clearColor;
  SDL_Point viewPos;
  scale_t scale; // Scale factors for X and Y
  std::vector<std::unique_ptr<QImage>> images;
  std::vector<std::unique_ptr<QLayer>> layers;
  std::forward_list<DebugRect> rects;
  std::forward_list<DebugLine> lines;

  std::array<float, 18> vertex_buffer;
  std::array<float, 12> texcoord_buffer;

  DCanvas(Canvas *canvas, SDL_Window *window);
  ~DCanvas();

  SPLAT_INLINE Image *CreateImage(SDL_Surface *surface);
  SPLAT_INLINE void DestroyImage(Image *image);

  SPLAT_INLINE void SetClearColor(const color_t &color);

  SPLAT_INLINE const SDL_Point &GetViewPosition() const { return viewPos; }
  SPLAT_INLINE void SetViewPosition(int x, int y);

  SPLAT_INLINE const scale_t &GetViewScale() const { return scale; }
  SPLAT_INLINE void SetViewScale(float x, float y);

  void Render();

  void DrawRect(SDL_Rect *rect, const color_t &color, unsigned int width, unsigned int ttl, bool filled, bool relative);
  void DrawLine(SDL_Point *start, SDL_Point *end, const color_t &color, unsigned int width, unsigned int ttl, bool relative);

  SPLAT_INLINE Layer *CreateLayer(std::string &name);
  SPLAT_INLINE void DestroyLayer(Layer *layerRef);
  SPLAT_INLINE void MoveLayerToTop(Layer *layerRef);
  SPLAT_INLINE void MoveLayer(Layer *layerRef, Layer *sibling);
};

Image *DCanvas::CreateImage(SDL_Surface *surface) {
  images.emplace_back(surface);
  return images.back().pointer;
}

void DCanvas::DestroyImage(Image *image) {
  if (image == nullptr) {
    throw BadParameterException();
  }

  images.remove_if([=](const std::unique_ptr<Image> &other) { return other.pointer() == image; });
}

void DCanvas::SetClearColor(const color_t &color) {
  glClearColor(color[0], color[1], color[2], color[3]);
}

Layer *DCanvas::CreateLayer() {
  layers.emplace(layers.cbegin());
  return layers.front().pointer();
}

Layer *DCanvas::CreateLayer(Layer *upper) {
  auto upperIt = GetLayerIterator(upper);
  if (upperIt == layers.cend()) {
    throw BadParameterException;
  }

  layers.emplace(upperIt);
  return layers.front().pointer();
}

void DCanvas::DestroyLayer(Layer *layer) {
  auto it = std::find_if(layers.cbegin(), layers.cend(), layer);
  if (it == layers.cend()) {
    throw BadParameterException();
  }

  layers.erase(it);
}

void DCanvas::MoveLayerToTop(Layer *layer) {
  shared_ptr<Layer> layer = layerRef.lock();

  auto cit = std::find(layers.cbegin(), layers.cend(), layer);
  if (cit == layers.cend()) {
    throw BadParameterException();
  }
  layers.erase(cit);

  layers.insert(layers.cbegin(), layer);
}

void DCanvas::MoveLayer(Layer *layerRef, Layer *sibling) {
  shared_ptr<Layer> layer = layerRef.lock();

  auto cit = std::find(layers.cbegin(), layers.cend(), layer);
  if (cit == layers.cend()) {
    throw BadParameterException();
  }
  layers.erase(cit);

  cit = std::find(layers.cbegin(), layers.cend(), sibling.lock());
  if (cit == layers.cend()) {
    throw BadParameterException();
  }
  layers.insert(cit, layer);
}

}

#endif // __LIB_SPLAT_SRC_DCANVAS_H__

