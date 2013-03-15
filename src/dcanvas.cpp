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

#include "dcanvas.h"

namespace Splat {

DCanvas::DCanvas(SDL_Window *window) : window(window) {
  window_glcontext = SDL_GL_CreateContext(window);
  if (!window_glcontext) {
    throw DriverException("OpenGL context creation failed");
  }

  // Our shading model--Flat
  glShadeModel(GL_FLAT);

  // Default the clear color to black.
  SetColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Setup our viewport.
  glViewport(0, 0, width, height);

  // Change to the projection matrix and set up our ortho view
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
#if __IPHONEOS__
  //glRotatef(-90.0f, 0.0f, 0.0f, 1.0f); // Rotate the display for landscape mode
  glOrthof(0, width, 0, height, -1.0f, 1.0f); //TODO this should really be for anything using OpenGL ES
#else
  gluOrtho2D(0, width, 0, height);
#endif

  // Set up modelview for 2D integer coordinates
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.375f, height + 0.375f, 0.0f);
  glScalef(1.0f, -1.0f, 0.001f); // Make the positive Z-axis point "out" from the view (e.g images at depth 4 will be higher than those at depth 0), and swap the Y axis

  glDisable(GL_DITHER);
}

DCanvas::~DCanvas() {
  SDL_GL_DeleteContext(window_glcontext);
  window_glcontext = nullptr;
}

Image *DCanvas::CreateImage(SDL_Surface *surface) {
  images.emplace_back(this, surface);
  return &images.back();
}

Layer *DCanvas::CreateLayer() {
  layers.emplace_back(this);
  return &layers.back();
}

void DCanvas::DestroyLayer(Layer *layer) {
  layers.remove_if(bool [=](Layer& candidate) { return candidate.d == layer->d; })
}

void DCanvas::SetColor(float r, float g, float b, float a) {
  glClearColor(color.r, color.g, color.b, color.a);
}

const SDL_Point *DCanvas::GetViewPosition() {
  return &viewPos;
}

void DCanvas::SetViewPosition(SDL_Point *position) {
  viewPos.x = position->x;
  viewPos.y = position->y;
}

void DCanvas::GetScale(const float &x, const float &y) {
  x = scale_x;
  y = scale_y;
}

void DCanvas::SetScale(float x, float y) {
  scale_x = x;
  scale_y = y;
}

void DCanvas::Render() {
}


