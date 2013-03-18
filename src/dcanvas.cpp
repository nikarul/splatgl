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
#include <GL/glu.h>
#include "splat.h"
#include "dcanvas.h"
#include "dimage.h"
#include "dinstance.h"

namespace Splat {

//TODO all exceptions must be part of the ABI
class SPLAT_PUBLIC OpenGLException : public DriverException {
public:
  GLenum error;

  OpenGLException(GLenum e) : DriverException("glGetErrorString(e)"), error(e) {}
};

DCanvas::DCanvas(Canvas *canvas, SDL_Window *window) : q(canvas), window(window) {
  glcontext = SDL_GL_CreateContext(window);
  if (!glcontext) {
    throw DriverException("OpenGL context creation failed");
  }

  // Our shading model--Flat
  glShadeModel(GL_FLAT);

  // Default the clear color to black.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Setup our viewport.
  int width, height;
  SDL_GetWindowSize(window, &width, &height);
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
  SDL_GL_DeleteContext(glcontext);
  glcontext = nullptr;
}

void DCanvas::Render() {
  int winWidth, winHeight;
  SDL_GetWindowSize(window, &winWidth, &winHeight);

  // Clear the color and depth buffers.
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  // Enable textures
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Save the current matrix
  glPushMatrix();

  // Scale as necessary
  glScalef(scale[0], scale[1], 1.0f);

  // Specify vertex and tex coord buffers
  glVertexPointer(3, GL_FLOAT, 0, vertex_buffer.data());
  glEnableClientState(GL_VERTEX_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, texcoord_buffer.data());
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  for (auto image = images.cbegin(); image != images.cend(); ++image) {
    const DImage &dimage(*(image->d));

    // Bind our texture
    glBindTexture(GL_TEXTURE_2D, dimage.texture);

    for (auto instance = dimage.instances.cbegin(); instance != dimage.instances.cend(); ++instance) {
      const DInstance &dinst(*(instance->d));

      // Save the current matrix
      glPushMatrix();

      // Translate to the images current location
      if (dinst.flags & FL_RELATIVE) {
        // Translate relative to the current view position
        glTranslatef(dinst.rect.x - viewPos.x, dinst.rect.y - viewPos.y, 0.0f);
      } else {
        glTranslatef(dinst.rect.x, dinst.rect.y, 0.0f);
      }
      // Set color for rendering
      glColor4f(dinst.color[0], dinst.color[1], dinst.color[2], dinst.color[3]);

      SDL_Rect scaled_rect = dinst.GetScaledRect();

      // Handle rotation
      if (dinst.flags & MASK_IMAGEMOD) {
        const float w2 = ((float) scaled_rect.w) * 0.5f;
        const float h2 = ((float) scaled_rect.h) * 0.5f;

        glTranslatef(w2, h2, 0.0f);

        if (dinst.flags & FL_ROTATE) { glRotatef(dinst.angle, 0.0f, 0.0f, 1.0f); }

        std::array<float, 16> matrix = { 
          1.0f, 0.0f, 0.0f, 0.0f, // COLUMN 1
          0.0f, 1.0f, 0.0f, 0.0f, // COLUMN 2
          0.0f, 0.0f, 1.0f, 0.0f, // COLUMN 3
          0.0f, 0.0f, 0.0f, 1.0f, // COLUMN 4
        };

        if (dinst.flags & FL_MIRROR_X) { matrix[0] = -1.0f; }
        if (dinst.flags & FL_MIRROR_Y) { matrix[5] = -1.0f; }
        if (dinst.flags & FL_ANTIDIAG) {
          matrix[4] = matrix[0];
          matrix[1] = matrix[5];
          matrix[0] = 0.0f;
          matrix[5] = 0.0f;
        }

        glMultMatrixf(matrix.data());

        glTranslatef(-w2, -h2, 0.0f);
      }

      // Handle scissoring
      if (dinst.flags & FL_CLIP) {
        SDL_Rect clip = dinst.GetClipRect();

        // Enable scissoring
        glEnable(GL_SCISSOR_TEST);

        // Snip, snip, snip...
        glScissor(clip.x, winHeight - (clip.y + clip.h), clip.w, clip.h);
      } else {
        // Disable scissoring
        glDisable(GL_SCISSOR_TEST);
      }

      const float depth = static_cast<float> (dinst.GetLayer());

      // Generate buffers for rendering

      // First triangle
      //glTexCoord2f(dinst.s1, dinst.t2);
      texcoord_buffer[0] = dinst.s1;
      texcoord_buffer[1] = dinst.t2;
      //glVertex3f(0.0f, scaled_rect.h, dinst.depth);
      vertex_buffer[0] = 0.0f;
      vertex_buffer[1] = scaled_rect.h;
      vertex_buffer[2] = depth;

      //glTexCoord2f(dinst.s1, dinst.t1);
      texcoord_buffer[2] = dinst.s1;
      texcoord_buffer[3] = dinst.t1;
      //glVertex3f(0.0f, 0.0f, dinst.depth);
      vertex_buffer[3] = 0.0f;
      vertex_buffer[4] = 0.0f;
      vertex_buffer[5] = depth;

      //glTexCoord2f(dinst.s2, dinst.t1);
      texcoord_buffer[4] = dinst.s2;
      texcoord_buffer[5] = dinst.t1;
      //glVertex3f(scaled_rect.w, 0.0f, dinst.depth);
      vertex_buffer[6] = scaled_rect.w;
      vertex_buffer[7] = 0.0f;
      vertex_buffer[8] = depth;

      // Second triangle
      //glTexCoord2f(dinst.s2, dinst.t2);
      texcoord_buffer[6] = dinst.s2;
      texcoord_buffer[7] = dinst.t2;
      //glVertex3f(scaled_rect.w, scaled_rect.h, dinst.depth);
      vertex_buffer[9] = scaled_rect.w;
      vertex_buffer[10] = scaled_rect.h;
      vertex_buffer[11] = depth;

      //glTexCoord2f(dinst.s1, dinst.t2);
      texcoord_buffer[8] = dinst.s1;
      texcoord_buffer[9] = dinst.t2;
      //glVertex3f(0.0f, scaled_rect.h, dinst.depth);
      vertex_buffer[12] = 0.0f;
      vertex_buffer[13] = scaled_rect.h;
      vertex_buffer[14] = depth;

      //glTexCoord2f(dinst.s2, dinst.t1);
      texcoord_buffer[10] = dinst.s2;
      texcoord_buffer[11] = dinst.t1;
      //glVertex3f(scaled_rect.w, 0.0f, dinst.depth);
      vertex_buffer[15] = scaled_rect.w;
      vertex_buffer[16] = 0.0f;
      vertex_buffer[17] = depth;
    }

    // Finished with our triangles
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Finished with our triangles
    glPopMatrix();
  }

  // Disable scissoring
  glDisable(GL_SCISSOR_TEST);

  const uint32_t time = SDL_GetTicks();

  // Draw rects
  if (!rects.empty())
  {
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, &vertex_buffer);

    auto prev = rects.cbefore_begin();
    for (auto r = rects.cbegin(); r != rects.cend(); /**/) {
      const DebugRect &prim(*r);

      if (time > prim.expire) {
        r = rects.erase_after(prev);
        continue;
      }

      glColor4f(prim.color[0], prim.color[1], prim.color[2], prim.color[3]);

      // Save the current matrix
      glPushMatrix();

      if (!prim.relative) {
        // Translate to the render context's current location
        glTranslatef(-viewPos.x, -viewPos.y, 0.0f);
      }

      glLineWidth(prim.width);

      // Prepare to render rects
      if (prim.filled) {
        // First triangle
        vertex_buffer[0] = prim.rect.x;
        vertex_buffer[1] = prim.rect.y;

        vertex_buffer[2] = prim.rect.x + prim.rect.w;
        vertex_buffer[3] = prim.rect.y;

        vertex_buffer[4] = prim.rect.x;
        vertex_buffer[5] = prim.rect.y + prim.rect.h;

        // Second triangle
        vertex_buffer[6] = prim.rect.x + prim.rect.w;
        vertex_buffer[7] = prim.rect.y + prim.rect.h;

        vertex_buffer[8] = prim.rect.y;
        vertex_buffer[9] = prim.rect.y + prim.rect.h;

        vertex_buffer[10] = prim.rect.x + prim.rect.w;
        vertex_buffer[11] = prim.rect.x;

        // Finished with our triangles
        glDrawArrays(GL_TRIANGLES, 0, 6);
      } else {
        vertex_buffer[0] = prim.rect.x;
        vertex_buffer[1] = prim.rect.y;

        vertex_buffer[2] = prim.rect.x + prim.rect.w;
        vertex_buffer[3] = prim.rect.y;

        vertex_buffer[4] = prim.rect.x + prim.rect.w;
        vertex_buffer[5] = prim.rect.y + prim.rect.h;

        vertex_buffer[6] = prim.rect.x;
        vertex_buffer[7] = prim.rect.y + prim.rect.h;

        // Finished with our lines
        glDrawArrays(GL_LINE_LOOP, 0, 4);
      }

      // Restore the old matrix
      glPopMatrix();

      ++r;
      ++prev;
    }
  }

  if (!lines.empty()) {
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    auto prev = lines.cbefore_begin();
    for (auto l = lines.begin(); l != lines.end(); /**/) {
      const DebugLine &prim(*l);

      if (time > prim.expire) {
        l = lines.erase_after(prev);
        continue;
      }

      glColor4f(prim.color[0], prim.color[1], prim.color[2], prim.color[3]);
      glLineWidth(prim.width);

      glVertexPointer(2, GL_FLOAT, 0, &prim.start.x);

      // Finished with our triangles
      glDrawArrays(GL_LINES, 0, 2);

      ++l;
      ++prev;
    }
  }

  // Restore original, non-scaled matrix
  glPopMatrix();

  // Finish rendering by swap buffers
  SDL_GL_SwapWindow(window);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    throw OpenGLException(err);
  }
}

void DCanvas::DrawRect(SDL_Rect *rect, const color_t &color, unsigned int width, unsigned int ttl, bool filled, bool relative) {
  rects.emplace_front();
  DebugRect &prim(rects.front());

  prim.rect = *rect;
  prim.color = color;
  prim.width = width;
  prim.expire = SDL_GetTicks() + ttl;
  prim.filled = filled;
  prim.relative = relative;
}

void DCanvas::DrawLine(SDL_Point *start, SDL_Point *end, const color_t &color, unsigned int width, unsigned int ttl, bool relative) {
  lines.emplace_front();
  DebugLine &prim(lines.front());

  prim.start.x = start->x;
  prim.start.y = start->y;
  prim.end.x = end->x;
  prim.end.y = end->y;
  prim.color = color;
  prim.width = width;
  prim.expire = SDL_GetTicks() + ttl;
  prim.relative = relative;
}

}

