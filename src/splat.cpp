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

#include "splat.h"
#include "context.h"

SDL_Window *window = NULL;
RenderContext *context = NULL;
SDL_GLContext window_glcontext = NULL;

// Public API
extern "C" {

extern DECLSPEC int SDLCALL Splat_Prepare(SDL_Window *userWindow, int viewportWidth, int viewportHeight) {
  if (context) {
    Splat_SetError("Splat already initialized!");
    return -1;
  }

	int width, height;
	window = userWindow;
  SDL_GetWindowSize(window, &width, &height);

  // Set up the default render context
  context = AllocContext("default");
  if (!context) {
    Splat_SetError(error, "Failed to allocate default render context.\n");
		Splat_Finish();
    return NULL;
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  window_glcontext = SDL_GL_CreateContext(window);
  if (!window_glcontext) {
		Splat_SetError("OpenGL context creation failed.  (Check OpenGL/SDL Error)");
		Splat_Finish();
    return -1;
  }

  // Our shading model--Flat
  glShadeModel(GL_FLAT);

  // Default the clear color to black.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Setup our viewport.
  glViewport(0, 0, viewportWidth, viewportHeight);

  // Change to the projection matrix and set up our ortho view
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, width, 0, height);

  // Set up modelview for 2D integer coordinates
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.375f, height + 0.375f, 0.0f);
  glScalef(1.0f, -1.0f, 0.001f); // Make the positive Z-axis point "out" from the view (e.g images at depth 4 will be higher than those at depth 0), and swap the Y axis

  /* Deactivate the system cursor */
  SDL_ShowCursor(SDL_DISABLE);

  glDisable(GL_DITHER);

  /* Create the frame buffer for rendering to texture*/
  glGenFramebuffers(1, &framebuffer);

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  /* Set up the texture to which we're going to render */
  glGenTextures(1, &frameTexture);
  glBindTexture(GL_TEXTURE_2D, frameTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  /* Configure the framebuffer texture */
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture, 0);
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, DrawBuffers);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
		Splat_SetError("OpenGL error occurred during initialization");
		Splat_Finish();
		return -1;
  }

	return 0;
}

extern DECLSPEC void SDLCALL Splat_Finish() {
	if (window) {
		SDL_GL_DeleteContext(window_glcontext);
		window_glcontext = NULL;
		window = NULL;
	}

  if (context) {
		ClearAllContexts();
		context = NULL;
  }
}

} // extern "C"
