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

#include "splat.h"
#include "dobject.h"

namespace Splat {

class SPLAT_LOCAL DCanvas : public DObject {
public:
  SDL_Window *window;

  DCanvas(SDL_Window *window);
  ~DCanvas();

  Image *CreateImage(SDL_Surface *surface);

  Layer *CreateLayer();
  void DestroyLayer(Layer *layer);

  void SetClearColor(float r, float g, float b, float a);
  const SDL_Point *GetViewPosition() const;
  void SetViewPosition(const SDL_Point *point);

  void GetScale(float &x, float &y);
  void SetScale(float x, float y);

  void Render();

  void DrawRect(SDL_Rect *rect, SDL_Color *color, int width = 1, int ttl = 0);
  void DrawSolidRect(SDL_Rect *rect, SDL_Color *color, int ttl = 0);
  void DrawLine(SDL_Point *start, SDL_Point *end, SDL_Color *color, int width = 1, int ttl = 0);
}

}

#endif // __LIB_SPLAT_SRC_CANVAS_H__

