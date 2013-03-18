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

#include <GL/gl.h>
#include "splat.h"
#include "dimage.h"

namespace Splat {

void DImage::Update(SDL_Surface *surface)
  GLenum format;

  // get the number of channels in the SDL surface
  if (surface->format->BytesPerPixel == 4) {     // contains an alpha channel
    if (surface->format->Rmask == 0x000000FF) {
      format = GL_RGBA;
    } else {
      format = GL_BGRA;
    }
  } else if (surface->format->BytesPerPixel == 3) {    // no alpha channel
    if (surface->format->Rmask == 0x000000FF) {
      format = GL_RGB;
    } else {
      format = GL_BGR;
    }
  } else {
    throw DriverError("SDL Surface is not true color (24 or 32-bit).");
  }

  if (SDL_MUSTLOCK(surface)) {
    if (!SDL_LockSurface(surface)) {
      throw DriverError("Failed to lock SDL surface to upload to OpenGL");
    }
  }

  // Have OpenGL generate a texture object handle for us
  glGenTextures(1, &texture);

  // Bind the texture object
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set the texture's stretching properties
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

  // Edit the texture object's image data using the information SDL_Surface gives us
  glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

  *width = surface->w;
  *height = surface->h;

  if (SDL_MUSTLOCK(surface)) {
    SDL_UnlockSurface(surface);
  }

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    throw OpenGLException(err);
  }
}

