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
#include <sstream>
#include "splat.h"
#include "canvas.h"
#include "types.h"

#define MASK_IMAGEMOD (SPLAT_MIRROR_X | SPLAT_MIRROR_Y | SPLAT_MIRROR_DIAG | SPLAT_ROTATE)

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

#define ERRCHECK() \
  { \
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) { \
      std::stringstream buffer("OpenGL error(s): "); \
      do { \
        buffer << err << " "; \
        err = glGetError(); \
      } while (err != GL_NO_ERROR); \
      buffer << "at " __FILE__ ":" << __LINE__; \
      Splat_SetError(buffer.str().c_str()); \
      return -1; \
    } \
  }

extern "C"
{

int Splat_Render(Splat_Canvas *canvas) {
  if (!canvas) {
    Splat_SetError("Splat_Render:  Invalid argument.");
    return -1;
  }

  int winwidth, winheight;
  SDL_Rect scaledRect;

  SDL_GetWindowSize(window, &winwidth, &winheight);

  /* Render to our framebuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); ERRCHECK();
  glViewport(0, 0, viewportWidth, viewportHeight); ERRCHECK();

  // Change to the projection matrix and set up our ortho view
  glMatrixMode(GL_PROJECTION); ERRCHECK();
  glLoadIdentity(); ERRCHECK();
  gluOrtho2D(0, viewportWidth, 0, viewportHeight); ERRCHECK();

  // Set up modelview for 2D integer coordinates
  glMatrixMode(GL_MODELVIEW); ERRCHECK();
  glLoadIdentity(); ERRCHECK();
  glTranslatef(0.375f, viewportHeight + 0.375f, 0.0f); ERRCHECK();
  glScalef(1.0f, -1.0f, 0.001f); ERRCHECK(); // Make the positive Z-axis point "out" from the view (e.g images at depth 4 will be higher than those at depth 0), and swap the Y axis

  // Clear the color and depth buffers.
  glClear(GL_COLOR_BUFFER_BIT); ERRCHECK();

  // Enable textures and blending
  glEnable(GL_TEXTURE_2D); ERRCHECK();
  glEnable(GL_BLEND); ERRCHECK();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); ERRCHECK();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, canvas->blending); ERRCHECK();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, canvas->blending); ERRCHECK();

  // Save the current matrix
  glPushMatrix(); ERRCHECK();

  // Scale as necessary
  glScalef(canvas->scale[0], canvas->scale[1], 1.0f); ERRCHECK();
  bool scaledCanvas = !(canvas->scale[0] == 1.0f && canvas->scale[1] == 1.0f);

  // Specify vertex and tex coord buffers
  glVertexPointer(3, GL_FLOAT, 0, vertex_buffer); ERRCHECK();
  glEnableClientState(GL_VERTEX_ARRAY); ERRCHECK();
  glTexCoordPointer(2, GL_FLOAT, 0, texcoord_buffer); ERRCHECK();
  glEnableClientState(GL_TEXTURE_COORD_ARRAY); ERRCHECK();

  SDL_Rect viewRect;
  viewRect.x = canvas->origin.x;
  viewRect.y = canvas->origin.y;
  viewRect.w = viewportWidth;
  viewRect.h = viewportHeight;

  float depth = 0.0f;
  for (Splat_Layer &layer : canvas->layers) {
    for (Splat_Instance &instance : layer.instances) {
      //TODO do this once per texture
      // Bind our texture
      glBindTexture(GL_TEXTURE_2D, instance.texture); ERRCHECK();

      if ((instance.flags & SPLAT_RELATIVE) != 0 && !SDL_HasIntersection(&instance.rect, &viewRect)) {
        continue;
      }

      // Save the current matrix
      glPushMatrix(); ERRCHECK();

      // If relative, translate to the active canvas' current location
      if (instance.flags & SPLAT_RELATIVE) {
        glTranslatef(-canvas->origin.x, -canvas->origin.y, 0.0f); ERRCHECK();
      }

      // Translate to the images current location
      glTranslatef(instance.rect.x, instance.rect.y, 0.0f); ERRCHECK();

      // Set color for rendering
      glColor4ub(instance.color.r, instance.color.g, instance.color.b, instance.color.a); ERRCHECK();

      // Scale the render rect
      scaledRect.x = instance.rect.x;
      scaledRect.y = instance.rect.y;
      scaledRect.w = instance.rect.w * instance.scale[0];
      scaledRect.h = instance.rect.h * instance.scale[1];

      // Handle rotation
      if (instance.flags & MASK_IMAGEMOD) {
        const float w2 = ((float) scaledRect.w) / 2.0f;
        const float h2 = ((float) scaledRect.h) / 2.0f;

        glTranslatef(w2, h2, 0.0f); ERRCHECK();

        if (instance.flags & SPLAT_MIRROR_DIAG) {
          glRotatef(-90.0f, 0.0f, 0.0f, 1.0f); ERRCHECK();
          if ((instance.flags & SPLAT_MIRROR_X)) {
            glScalef(1.0f, -1.0f, 1.0f); ERRCHECK();
          }
          if ((instance.flags & SPLAT_MIRROR_Y) == 0) {
            glScalef(-1.0f, 1.0f, 1.0f); ERRCHECK();
          }
        } else {
          if (instance.flags & SPLAT_MIRROR_X) {
            glScalef(-1.0f, 1.0f, 1.0f); ERRCHECK();
          }
          if (instance.flags & SPLAT_MIRROR_Y) {
             glScalef(1.0f, -1.0f, 1.0f); ERRCHECK();
          }
        }
        if (instance.flags & SPLAT_ROTATE) {
          glRotatef(instance.angle, 0.0f, 0.0f, 1.0f); ERRCHECK();
        }

        glTranslatef(-w2, -h2, 0.0f); ERRCHECK();
      }

      // Handle scissoring
      if (!SDL_RectEmpty(&instance.clip)) {
        // Enable scissoring
        glEnable(GL_SCISSOR_TEST); ERRCHECK();

        // Snip, snip, snip...
        if (scaledCanvas) {
          glScissor(instance.clip.x * canvas->scale[0], winheight - ((instance.clip.y + instance.clip.h) * canvas->scale[1]), instance.clip.w * canvas->scale[0], instance.clip.h * canvas->scale[1]); ERRCHECK();
        } else {
          glScissor(instance.clip.x, winheight - (instance.clip.y + instance.clip.h), instance.clip.w, instance.clip.h); ERRCHECK();
        }
      } else {
        // Disable scissoring
        glDisable(GL_SCISSOR_TEST); ERRCHECK();
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
      glDrawArrays(GL_TRIANGLES, 0, 6); ERRCHECK();

      // Finished with our triangles
      glPopMatrix(); ERRCHECK();
    }

    depth += 1.0f;
  }

  // Disable scissoring
  glDisable(GL_SCISSOR_TEST); ERRCHECK();

  uint32_t time = SDL_GetTicks();

  // Draw rects
  if (!canvas->rects.empty()) {
    glDisable(GL_TEXTURE_2D); ERRCHECK();
    glEnableClientState(GL_VERTEX_ARRAY); ERRCHECK();
    glDisableClientState(GL_TEXTURE_COORD_ARRAY); ERRCHECK();
    glVertexPointer(2, GL_FLOAT, 0, &vertex_buffer); ERRCHECK();

    for (auto itPrev = canvas->rects.before_begin(), it = canvas->rects.begin(); it != canvas->rects.end(); /**/) {
      const Splat_Rect& rect = *it;
      glColor4ub(rect.color.r, rect.color.g, rect.color.b, rect.color.a); ERRCHECK();

      // Save the current matrix
      glPushMatrix(); ERRCHECK();

      if (!rect.relative) {
        // Translate to the active canvas's current location
        glTranslatef(-canvas->origin.x, -canvas->origin.y, 0.0f); ERRCHECK();
      }

      glLineWidth(rect.width); ERRCHECK();

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
        glDrawArrays(GL_TRIANGLES, 0, 6); ERRCHECK();
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
        glDrawArrays(GL_LINE_LOOP, 0, 4); ERRCHECK();
      }

      // Restore the old matrix
      glPopMatrix(); ERRCHECK();

      // Expire old rects
      if (time >= rect.ttl) {
        it = canvas->rects.erase_after(itPrev);
      } else {
        itPrev = it;
        ++it;
      }
    }
  }

  if (!canvas->lines.empty()) {
    glDisable(GL_TEXTURE_2D); ERRCHECK();
    glEnableClientState(GL_VERTEX_ARRAY); ERRCHECK();
    glDisableClientState(GL_TEXTURE_COORD_ARRAY); ERRCHECK();
    for (auto itPrev = canvas->lines.before_begin(), it = canvas->lines.begin(); it != canvas->lines.end(); /**/) {
      const Splat_Line &line = *it;
      glColor4ub(line.color.r, line.color.g, line.color.b, line.color.a); ERRCHECK();

      if (!line.relative) {
        // Translate to the active canvas's current location
        glTranslatef(-canvas->origin.x, -canvas->origin.y, 0.0f); ERRCHECK();
      }

      glLineWidth(line.width); ERRCHECK();

      glVertexPointer(2, GL_FLOAT, 0, &line.start.x); ERRCHECK();

      // Finished with our triangles
      glDrawArrays(GL_LINES, 0, 2); ERRCHECK();

      // Expire old lines
      if (time >= line.ttl) {
        it = canvas->lines.erase_after(itPrev);
      } else {
        itPrev = it;
        ++it;
      }
    }
  }

  // Restore original, non-scaled matrix
  glPopMatrix(); ERRCHECK();

  // Render to the screen
  glBindFramebuffer(GL_FRAMEBUFFER, 0); ERRCHECK();
  glViewport(0, 0, winwidth, winheight); ERRCHECK(); // Render on the whole framebuffer, complete from the lower left corner to the upper right

  glDisable(GL_BLEND); ERRCHECK();
  glEnable(GL_TEXTURE_2D); ERRCHECK();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); ERRCHECK();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); ERRCHECK();

  // Specify vertex and tex coord buffers
  glVertexPointer(3, GL_FLOAT, 0, vertex_buffer); ERRCHECK();
  glEnableClientState(GL_VERTEX_ARRAY); ERRCHECK();
  glTexCoordPointer(2, GL_FLOAT, 0, texcoord_buffer); ERRCHECK();
  glEnableClientState(GL_TEXTURE_COORD_ARRAY); ERRCHECK();

  // Change to the projection matrix and set up our ortho view
  glMatrixMode(GL_PROJECTION); ERRCHECK();
  glLoadIdentity(); ERRCHECK();
  gluOrtho2D(0, winwidth, 0, winheight); ERRCHECK();

  // Set up modelview for 2D integer coordinates
  glMatrixMode(GL_MODELVIEW); ERRCHECK();
  glLoadIdentity(); ERRCHECK();
  glTranslatef(0.375f, winheight + 0.375f, 0.0f); ERRCHECK();
  glScalef(1.0f, -1.0f, 0.001f); ERRCHECK(); // Make the positive Z-axis point "out" from the view (e.g images at depth 4 will be higher than those at depth 0), and swap the Y axis

  glBindTexture(GL_TEXTURE_2D, frameTexture); ERRCHECK();
  glColor4ub(255, 255, 255, 255); ERRCHECK();

  glUseProgram(shaderProgram); ERRCHECK();

  if (shaderProgram) {
    float size[2];
    GLint uniform;

    size[0] = viewportWidth;
    size[1] = viewportHeight;
    uniform = glGetUniformLocation(shaderProgram, "rubyInputSize"); ERRCHECK();
    glUniform2fv(uniform, 1, size); ERRCHECK();

    uniform = glGetUniformLocation(shaderProgram, "rubyTextureSize"); ERRCHECK();
    glUniform2fv(uniform, 1, size); ERRCHECK();

    size[0] = winwidth;
    size[1] = winheight;
    uniform = glGetUniformLocation(shaderProgram, "rubyOutputSize"); ERRCHECK();
    glUniform2fv(uniform, 1, size); ERRCHECK();
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
  glDrawArrays(GL_TRIANGLES, 0, 6); ERRCHECK();

  glUseProgram(0); ERRCHECK();

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

