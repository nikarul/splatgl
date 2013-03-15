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

#ifndef __LIB_SPLAT_SRC_DIMAGE_H__
#define __LIB_SPLAT_SRC_DIMAGE_H__

#include "splat.h"
#include "dobject.h"

namespace Splat {

class SPLAT_LOCAL DImage : public DObject {
public:
  GLuint texture;
  uint32_t width;
  uint32_t height;

  forward_list<Instance> instances;


  Instance *CreateInstance(Layer *Layer, int x, int y, SDL_Rect *subimage = nullptr);

  void Update(SDL_Surface *surface);
  int GetWidth();
  int GetHeight();
};

}

#endif // __LIB_SPLAT_SRC_DIMAGE_H__

