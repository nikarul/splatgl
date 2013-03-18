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

#ifndef __LIB_SPLAT_SRC_DINSTANCE_H__
#define __LIB_SPLAT_SRC_DINSTANCE_H__

#include "splat.h"
#include "dobject.h"

namespace Splat {

constexpr uint32_t FL_MIRROR_X(0x01);
constexpr uint32_t FL_MIRROR_Y(0x02);
constexpr uint32_t FL_ANTIDIAG(0x04);
constexpr uint32_t FL_RELATIVE(0x08);
constexpr uint32_t FL_ROTATE(0x10);
constexpr uint32_t FL_CLIP(0x20);
constexpr uint32_t FL_HIDDEN(0x40);
constexpr uint32_t MASK_IMAGEMOD(FL_MIRROR_X | FL_MIRROR_Y | FL_ANTIDIAG | FL_ROTATE);

class SPLAT_LOCAL DInstance : public DObject {
public:
  GLuint texture;
  SDL_Rect rect;
  float s1;
  float t1;
  float s2;
  float t2;
  int layer;
  float angle; /* Rotation angle to apply when rendering this image */
  scale_t scale;
  color_t color; /* Color to use when rendering the image. */
  uint32_t flags;
  SDL_Rect clip; /* FL_CLIPPED is set, the image is clipped to this rect */


  SPLAT_INLINE SDL_Rect GetScaledRect() const { return rect; }

  SPLAT_INLINE SDL_Rect GetClipRect() const { return clip; }
  SPLAT_INLINE void SetClipRect(const SDL_Rect *rect) const;

  SPLAT_INLINE SDL_Rect GetRect() const { return rect; }

  SPLAT_INLINE void SetColor(const color_t& color) { this->color = color; }

  SPLAT_INLINE void SetAngle(float angle) { this->angle = angle; }

  SPLAT_INLINE scale_t GetScale() const { return scale; }
  SPLAT_INLINE void SetScale(const scale_t &scale) { this->scale = scale; }

  SPLAT_INLINE bool IsVisible() const { return (flags & FL_HIDDEN) == 0; }
  SPLAT_INLINE void SetVisible(bool visible);

  SPLAT_INLINE bool IsRelativeToViewPosition() const { return (flags & FL_RELATIVE) == FL_RELATIVE; }
  SPLAT_INLINE void SetRelativeToViewPosition(bool visible);

  SPLAT_INLINE int GetLayer() const { return layer; }
};

}

#endif // __LIB_SPLAT_SRC_DINSTANCE_H__

