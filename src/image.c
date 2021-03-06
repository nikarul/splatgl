/*
  Splat Graphics Library
  Copyright (C) 2014  Michael Dale Long <mlong@digitalbytes.net>
  http://digitalbytes.net/projects/splatgl/

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

static Splat_Image *images = NULL;

Splat_Image *Splat_CreateImage(SDL_Surface *surface) {
  if (!surface) {
    Splat_SetError("Splat_CreateImage:  Invalid argument.");
    return NULL;
  }

  // Allocate the surface for this context
  Splat_Image *image = malloc(sizeof(Splat_Image));
  if (!image) {
    Splat_SetError("Splat_CreateImage:  Allocation failed.");
    return NULL;
  }

  // Place new image at the top of the list.
  image->next = images;
  images = image;

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
    return NULL;
  }

  if (SDL_MUSTLOCK(surface)) {
    if (!SDL_LockSurface(surface)) {
      Splat_SetError("Failed to lock surface to upload to OpenGL");
      return NULL;
    }
  }

  // Have OpenGL generate a texture object handle for us
  glGenTextures(1, &image->texture);

  // Bind the texture object
  glBindTexture(GL_TEXTURE_2D, image->texture);

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
  image->width = surface->w;
  image->height = surface->h;

  return image;
}

int Splat_UpdateImage(Splat_Image *image, SDL_Surface *surface) {
  if (!surface || !image) {
    Splat_SetError("Splat_CreateImage:  Invalid argument.");
    return 1;
  }

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
    return 1;
  }

  if (SDL_MUSTLOCK(surface)) {
    if (!SDL_LockSurface(surface)) {
      Splat_SetError("Failed to lock surface to upload to OpenGL");
      return 1;
    }
  }

  // Bind the texture object
  glBindTexture(GL_TEXTURE_2D, image->texture);

  // Edit the texture object's surface data using the information SDL_Surface gives us
  glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

  if (SDL_MUSTLOCK(surface)) {
    SDL_UnlockSurface(surface);
  }

  // Set the surface width and height
  image->width = surface->w;
  image->height = surface->h;

  return 0;
}

int Splat_DestroyImage(Splat_Image *image) {
  if (!image) {
    Splat_SetError("Splat_DestroyImage:  Invalid argument.");
    return -1;
  }

  for (Splat_Image *prev = NULL, *curr = images; curr != NULL; prev = curr, curr = curr->next) {
    if (curr == image) {
      if (prev) {
        prev->next = curr->next;
      } else {
        images = curr->next;
      }

      glDeleteTextures(1, &image->texture);
      free(image);
      return 0;
    }
  }

  Splat_SetError("Splat_DestroyImage:  Image not found.");
  return -1;
}

int Splat_GetImageSize(Splat_Image *image, uint32_t *width, uint32_t *height) {
  *width = image->width;
  *height = image->height;
  return 0;
}

