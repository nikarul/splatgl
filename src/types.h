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

#ifndef __SPLAT_TYPES_H__
#define __SPLAT_TYPES_H__

#include <stdbool.h>
#include <SDL_opengl.h>
#include <SDL.h>

typedef struct Splat_Image {
  GLuint texture;
  uint32_t width;
  uint32_t height;
  struct Splat_Image *next;
} Splat_Image;

typedef struct Splat_Instance {
  GLuint texture;
  SDL_Rect rect;
  float s1;
  float t1;
  float s2;
  float t2;
  Splat_Layer *layer;
  float angle; /* Rotation angle to apply when renderering this image */
  float scale[2];
  SDL_Color color; /* Color to use when renderering the image. */
  uint32_t flags;
  Splat_Instance *nextCulledHandle;
  SDL_Rect clip; /* If not empty, the image is clipped to this rect */
  Splat_Instance *next;
} Splat_Instance;

typedef struct Splat_Layer {
  Splat_Canvas *canvas;
  Splat_Instance *instances;
  struct Splat_Layer *next;
} Splat_Layer;

typedef struct Splat_Rect {
  int x1;
  int y1;
  int x2;
  int y2;
  int width;
  SDL_Color color;
  uint32_t ttl;
  bool relative;
  bool fill;
  struct Splat_Rect *next;
} Splat_Rect;

typedef struct Splat_Line {
  SDL_Point start;
	SDL_Point end;
	SDL_Color color;
	int width;
	uint32_t ttl;
  bool relative;
  struct Splat_Line *next;
} Splat_Line;

//struct Splat_Shader {
//  int type;
//  GLuint shader;
//};
//
//struct Splat_Program {
//  GLuint program;
//};

#endif // __SPLAT_TYPES_H__

