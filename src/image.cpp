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

#include <SDL.h>
#include <SDL_opengl.h>
#include "splat.h"
#include "types.h"
#include "canvas.h"

extern "C" {

Splat_Image *Splat_CreateImage(SDL_Surface *surface) {
  // Ensure we have a valid canvas
  if (!activeCanvas) {
	Splat_SetError("No active canvas");
	return nullptr;
  }

  // ALlocate the surface for this context
  activeCanvas->images.emplace_back();
  Splat_Image &image(activeCanvas->images.back());

  // Get the number of channels in the SDL surface
  GLenum format;
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
	Splat_SetError("SDL_Surface is not true color (24 or 32-bit).");
	return nullptr;
  }

  if (SDL_MUSTLOCK(surface)) {
	if (!SDL_LockSurface(surface)) {
	  Splat_SetError("Failed to lock surface to upload to OpenGL");
	  return nullptr;
	}
  }

  // Have OpenGL generate a texture object handle for us
  glGenTextures(1, &image.texture);

  // Bind the texture object
  glBindTexture(GL_TEXTURE_2D, image.texture);

  // Set the texture's stretching properties
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

  // Edit the texture object's surface data using the information SDL_Surface gives us
  glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

  if (SDL_MUSTLOCK(surface)) {
	SDL_UnlockSurface(surface);
  }

  // Set the surface width and height
  image.width = surface->w;
  image.height = surface->h;

  return &image;
}

} // extern "C"

