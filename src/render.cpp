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

#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>
#include <GL/glu.h>

#include <algorithm>
#include "splat.h"
#include "canvas.h"
#include "types.h"

#define MASK_IMAGEMOD (FL_MIRROR_X | FL_MIRROR_Y | FL_MIRROR_DIAG | FL_ROTATE)

SDL_Window *window = nullptr;
SDL_GLContext window_glcontext = nullptr;
GLuint framebuffer = 0;
GLuint frameTexture = 0;
int viewportWidth = 0;
int viewportHeight = 0;
GLuint vertexShader = 0;
GLuint fragmentShader = 0;
GLuint shaderProgram = 0;

static float vertex_buffer[18]; /* Vertex buffer */
static float texcoord_buffer[12]; /* TexCoord buffer */

extern "C"
{

int Splat_Render() {
  int winwidth, winheight;
  SDL_Rect scaledRect;

  SDL_GetWindowSize(window, &winwidth, &winheight);

  /* Render to our framebuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glViewport(0, 0, viewportWidth, viewportHeight);

  // Change to the projection matrix and set up our ortho view
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewportWidth, 0, viewportHeight);

  // Set up modelview for 2D integer coordinates
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.375f, viewportHeight + 0.375f, 0.0f);
  glScalef(1.0f, -1.0f, 0.001f); // Make the positive Z-axis point "out" from the view (e.g images at depth 4 will be higher than those at depth 0), and swap the Y axis

  // Clear the color and depth buffers.
  glClear(GL_COLOR_BUFFER_BIT);

  // Enable textures and blending
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, activeCanvas->blending);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, activeCanvas->blending);

  // Save the current matrix
  glPushMatrix();

  // Scale as necessary
  glScalef(activeCanvas->scale[0], activeCanvas->scale[1], 1.0f);
  bool scaledCanvas = !(activeCanvas->scale[0] == 1.0f && activeCanvas->scale[1] == 1.0f);

  // Specify vertex and tex coord buffers
  glVertexPointer(3, GL_FLOAT, 0, vertex_buffer);
  glEnableClientState(GL_VERTEX_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, texcoord_buffer);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  SDL_Rect viewRect;
  viewRect.x = activeCanvas->origin.x;
  viewRect.y = activeCanvas->origin.y;
  viewRect.w = viewportWidth;
  viewRect.h = viewportHeight;

  float depth = 0.0f;
  for (Splat_Layer &layer : activeCanvas->layers) {
    for (Splat_Instance &instance : layer.instances) {
      //TODO do this once per texture
      // Bind our texture
      glBindTexture(GL_TEXTURE_2D, instance.texture);

      if ((instance.flags & FL_RELATIVE) != 0 && !SDL_HasIntersection(&instance.rect, &viewRect)) {
        continue;
      }

      // Save the current matrix
      glPushMatrix();

      // If relative, translate to the active canvas' current location
      if (instance.flags & FL_RELATIVE) {
        glTranslatef(-activeCanvas->origin.x, -activeCanvas->origin.y, 0.0f);
      }

      // Translate to the images current location
      glTranslatef(instance.rect.x, instance.rect.y, 0.0f);

      // Set color for rendering
      glColor4ub(instance.color.r, instance.color.g, instance.color.b, instance.color.a);

      // Scale the render rect
      scaledRect.x = instance.rect.x;
      scaledRect.y = instance.rect.y;
      scaledRect.w = instance.rect.w * instance.scale[0];
      scaledRect.h = instance.rect.h * instance.scale[1];

      // Handle rotation
      if (instance.flags & MASK_IMAGEMOD) {
        const float w2 = ((float) scaledRect.w) / 2.0f;
        const float h2 = ((float) scaledRect.h) / 2.0f;

        glTranslatef(w2, h2, 0.0f);

        if (instance.flags & FL_MIRROR_DIAG) {
          glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
          if ((instance.flags & FL_MIRROR_X)) {
            glScalef(1.0f, -1.0f, 1.0f);
          }
          if ((instance.flags & FL_MIRROR_Y) == 0) {
            glScalef(-1.0f, 1.0f, 1.0f);
          }
        } else {
          if (instance.flags & FL_MIRROR_X) {
            glScalef(-1.0f, 1.0f, 1.0f);
          }
          if (instance.flags & FL_MIRROR_Y) {
             glScalef(1.0f, -1.0f, 1.0f);
          }
        }
        if (instance.flags & FL_ROTATE) {
          glRotatef(instance.angle, 0.0f, 0.0f, 1.0f);
        }

        glTranslatef(-w2, -h2, 0.0f);
      }

      // Handle scissoring
      if (!SDL_RectEmpty(&instance.clip)) {
        // Enable scissoring
        glEnable(GL_SCISSOR_TEST);

        // Snip, snip, snip...
        if (scaledCanvas) {
          glScissor(instance.clip.x * activeCanvas->scale[0], winheight - ((instance.clip.y + instance.clip.h) * activeCanvas->scale[1]), instance.clip.w * activeCanvas->scale[0], instance.clip.h * activeCanvas->scale[1]);
        } else {
          glScissor(instance.clip.x, winheight - (instance.clip.y + instance.clip.h), instance.clip.w, instance.clip.h);
        }
      } else {
        // Disable scissoring
        glDisable(GL_SCISSOR_TEST);
      }

      // Prepare to render triangles

      // First triangle
      //glTexCoord2f(instance.s1, instance.t2);
      texcoord_buffer[0] = instance.s1;
      texcoord_buffer[1] = instance.t2;
      //glVertex3f(0.0f, scaled_rect.h, depth);
      vertex_buffer[0] = 0.0f;
      vertex_buffer[1] = scaledRect.h;
      vertex_buffer[2] = depth;

      //glTexCoord2f(instance.s1, instance.t1);
      texcoord_buffer[2] = instance.s1;
      texcoord_buffer[3] = instance.t1;
      //glVertex3f(0.0f, 0.0f, depth);
      vertex_buffer[3] = 0.0f;
      vertex_buffer[4] = 0.0f;
      vertex_buffer[5] = depth;

      //glTexCoord2f(instance.s2, instance.t1);
      texcoord_buffer[4] = instance.s2;
      texcoord_buffer[5] = instance.t1;
      //glVertex3f(scaled_rect.w, 0.0f, depth);
      vertex_buffer[6] = scaledRect.w;
      vertex_buffer[7] = 0.0f;
      vertex_buffer[8] = depth;

      // Second triangle
      //glTexCoord2f(instance.s2, instance.t2);
      texcoord_buffer[6] = instance.s2;
      texcoord_buffer[7] = instance.t2;
      //glVertex3f(scaled_rect.w, scaled_rect.h, depth);
      vertex_buffer[9] = scaledRect.w;
      vertex_buffer[10] = scaledRect.h;
      vertex_buffer[11] = depth;

      //glTexCoord2f(instance.s1, instance.t2);
      texcoord_buffer[8] = instance.s1;
      texcoord_buffer[9] = instance.t2;
      //glVertex3f(0.0f, scaled_rect.h, depth);
      vertex_buffer[12] = 0.0f;
      vertex_buffer[13] = scaledRect.h;
      vertex_buffer[14] = depth;

      //glTexCoord2f(instance.s2, instance.t1);
      texcoord_buffer[10] = instance.s2;
      texcoord_buffer[11] = instance.t1;
      //glVertex3f(scaled_rect.w, 0.0f, depth);
      vertex_buffer[15] = scaledRect.w;
      vertex_buffer[16] = 0.0f;
      vertex_buffer[17] = depth;

      // Finished with our triangles
      glDrawArrays(GL_TRIANGLES, 0, 6);

      // Finished with our triangles
      glPopMatrix();
    }

    depth += 1.0f;
  }

  // Disable scissoring
  glDisable(GL_SCISSOR_TEST);

  uint32_t time = SDL_GetTicks();

  // Draw rects
  if (!activeCanvas->rects.empty()) {
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, &vertex_buffer);

    for (auto itPrev = activeCanvas->rects.before_begin(), it = activeCanvas->rects.begin(); it != activeCanvas->rects.end(); /**/) {
      const Splat_Rect& rect = *it;
      glColor4ub(rect.color.r, rect.color.g, rect.color.b, rect.color.a);

      // Save the current matrix
      glPushMatrix();

      if (!rect.relative) {
        // Translate to the active canvas's current location
        glTranslatef(-activeCanvas->origin.x, -activeCanvas->origin.y, 0.0f);
      }

      glLineWidth(rect.width);

      // Prepare to render rects
      if (rect.fill) {
        // First triangle
        vertex_buffer[0] = rect.x1;
        vertex_buffer[1] = rect.y1;

        vertex_buffer[2] = rect.x2;
        vertex_buffer[3] = rect.y1;

        vertex_buffer[4] = rect.x1;
        vertex_buffer[5] = rect.y2;

        // Second triangle
        vertex_buffer[6] = rect.x2;
        vertex_buffer[7] = rect.y2;

        vertex_buffer[8] = rect.y1;
        vertex_buffer[9] = rect.y2;

        vertex_buffer[10] = rect.x2;
        vertex_buffer[11] = rect.x1;

        // Finished with our triangles
        glDrawArrays(GL_TRIANGLES, 0, 6);
      } else {
        vertex_buffer[0] = rect.x1;
        vertex_buffer[1] = rect.y1;

        vertex_buffer[2] = rect.x2;
        vertex_buffer[3] = rect.y1;

        vertex_buffer[4] = rect.x2;
        vertex_buffer[5] = rect.y2;

        vertex_buffer[6] = rect.x1;
        vertex_buffer[7] = rect.y2;

        // Finished with our lines
        glDrawArrays(GL_LINE_LOOP, 0, 4);
      }

      // Restore the old matrix
      glPopMatrix();

      // Expire old rects
      if (time >= rect.ttl) {
        it = activeCanvas->rects.erase_after(itPrev);
      } else {
        itPrev = it;
        ++it;
      }
    }
  }

  if (!activeCanvas->lines.empty()) {
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    for (auto itPrev = activeCanvas->lines.before_begin(), it = activeCanvas->lines.begin(); it != activeCanvas->lines.end(); /**/) {
      const Splat_Line &line = *it;
      glColor4ub(line.color.r, line.color.g, line.color.b, line.color.a);
      glLineWidth(line.width);

      glVertexPointer(2, GL_FLOAT, 0, &line.start.x);

      // Finished with our triangles
      glDrawArrays(GL_LINES, 0, 2);

      // Expire old lines
      if (time >= line.ttl) {
        it = activeCanvas->lines.erase_after(itPrev);
      } else {
        itPrev = it;
        ++it;
      }
    }
  }

  // Restore original, non-scaled matrix
  glPopMatrix();

  // Render to the screen
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, winwidth, winheight); // Render on the whole framebuffer, complete from the lower left corner to the upper right

  glDisable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Specify vertex and tex coord buffers
  glVertexPointer(3, GL_FLOAT, 0, vertex_buffer);
  glEnableClientState(GL_VERTEX_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, texcoord_buffer);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  // Change to the projection matrix and set up our ortho view
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, winwidth, 0, winheight);

  // Set up modelview for 2D integer coordinates
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.375f, winheight + 0.375f, 0.0f);
  glScalef(1.0f, -1.0f, 0.001f); // Make the positive Z-axis point "out" from the view (e.g images at depth 4 will be higher than those at depth 0), and swap the Y axis

  glBindTexture(GL_TEXTURE_2D, frameTexture);
  glColor4ub(255, 255, 255, 255);

  glUseProgram(shaderProgram);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    Splat_SetError("glUseProgram() failed");
    return -1;
  }

  if (shaderProgram) {
    float size[2];
    GLint uniform;

    size[0] = viewportWidth;
    size[1] = viewportHeight;
    uniform = glGetUniformLocation(shaderProgram, "rubyInputSize");
    glUniform2fv(uniform, 1, size);

    uniform = glGetUniformLocation(shaderProgram, "rubyTextureSize");
    glUniform2fv(uniform, 1, size);

    size[0] = winwidth;
    size[1] = winheight;
    uniform = glGetUniformLocation(shaderProgram, "rubyOutputSize");
    glUniform2fv(uniform, 1, size);
  }

  // First triangle
  //glTexCoord2f(handle->s1, handle->t2);
  texcoord_buffer[0] = 0.0f;
  texcoord_buffer[1] = 0.0f;
  //glVertex3f(0.0f, scaled_rect.h, handle->depth);
  vertex_buffer[0] = 0.0f;
  vertex_buffer[1] = (float) winheight;
  vertex_buffer[2] = 0.0f;

  //glTexCoord2f(handle->s1, handle->t1);
  texcoord_buffer[2] = 0.0f;
  texcoord_buffer[3] = 1.0f;
  //glVertex3f(0.0f, 0.0f, handle->depth);
  vertex_buffer[3] = 0.0f;
  vertex_buffer[4] = 0.0f;
  vertex_buffer[5] = 0.0f;

  //glTexCoord2f(handle->s2, handle->t1);
  texcoord_buffer[4] = 1.0f;
  texcoord_buffer[5] = 1.0f;
  //glVertex3f(scaled_rect.w, 0.0f, handle->depth);
  vertex_buffer[6] = (float) winwidth;
  vertex_buffer[7] = 0.0f;
  vertex_buffer[8] = 0.0f;

  // Second triangle
  //glTexCoord2f(handle->s2, handle->t2);
  texcoord_buffer[6] = 1.0f;
  texcoord_buffer[7] = 0.0f;
  //glVertex3f(scaled_rect.w, scaled_rect.h, handle->depth);
  vertex_buffer[9] = (float) winwidth;
  vertex_buffer[10] = (float) winheight;
  vertex_buffer[11] = 0.0f;

  //glTexCoord2f(handle->s1, handle->t2);
  texcoord_buffer[8] = 0.0f;
  texcoord_buffer[9] = 0.0f;
  //glVertex3f(0.0f, scaled_rect.h, handle->depth);
  vertex_buffer[12] = 0.0f;
  vertex_buffer[13] = (float) winheight;
  vertex_buffer[14] = 0.0f;

  //glTexCoord2f(handle->s2, handle->t1);
  texcoord_buffer[10] = 1.0f;
  texcoord_buffer[11] = 1.0f;
  //glVertex3f(scaled_rect.w, 0.0f, handle->depth);
  vertex_buffer[15] = (float) winwidth;
  vertex_buffer[16] = 0.0f;
  vertex_buffer[17] = 0.0f;

  // Finished with our triangles
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glUseProgram(0);

  // Finish rendering by swap buffers
  SDL_GL_SwapWindow(window);

#if ENABLE_FPS_LOG
  fps_lastframe = fps_frametime;
  fps_frametime = SDL_GetTicks();
  Uint32 current_fps = 1.0f / (((float) (fps_frametime - fps_lastframe)) / 1000.0f);
  fps_max = current_fps > fps_max ? current_fps : fps_max;
  fps_min = current_fps < fps_min ? current_fps : fps_min;
  fps_times[fps_index] = current_fps;
  fps_index++;
  if (fps_index > MAX_FPS_INDEX) {
    fps_index = 0;
  }

  if (fps_frametime > fps_next_update) {
    Uint32 i, total = 0, fps;
    for (i = 0; i < MAX_FPS_INDEX; i++) {
      total += fps_times[i];
    }
    fps = total / MAX_FPS_INDEX;
    printf("FPS:  %d (%d to %d)\n", fps, fps_min, fps_max);

    // Reset min/max and set next print time
    fps_min = (Uint32) -1;
    fps_max = 0;
    fps_next_update = fps_frametime + 2000;
  }
#endif

  return 0;
}

} // extern "C"

