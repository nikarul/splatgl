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

class SPLAT_LOCAL DInstance : public DObject {
public:
  GLuint texture;
  SDL_Rect rect;
  float s1;
  float t1;
  float s2;
  float t2;
  float depth;
  float angle; /* Rotation angle to apply when renderering this image */
  float scale[2];
  color_t color; /* Color to use when renderering the image. */
  uint32_t flags;
  SDL_Rect clip; /* FL_CLIPPED is set, the image is clipped to this rect */


  SDL_Rect GetScaledRect();
  SDL_Rect GetClipRect();
//  GetOrigin()
//  SetExtents()
//  GetRect()
//  GetClipRect()
//  SetClipRect()
//  SetColor()
//  SetAngle()
//  GetScale()
//  SetScale()
//
//  IsVisible()
//  SetVisible()
//
//  IsRelativeToViewPosition()
//  SetRelativeToViewPosition();
};

}

#endif // __LIB_SPLAT_SRC_DINSTANCE_H__

