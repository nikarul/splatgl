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

/**
 * \file splat.h 
 *  
 * Main include header for Splat
 */

/**
 *  \mainpage Splat 2D Rendering Library
 *  
 *  http://www.digitalbytes.net/libsplat/
 *  
 *  \section intro_sec Introduction
 *  
 *  This is Splat, an SDL-based 2D sprite/tile rendering library.
 *  
 *  Splat is written in C++, but provides a C API as well.
 *  
 *  This library is distributed under Zlib License, found in the file "COPYING".
 *  
 */

#include <SDL2/begin_code.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Prepares Splat for rendering.
 *  
 *  The window argument is an SDL window already created by the
 *  application for Splat to use.
 *  
 *  Returns 0 if successful, 1 otherwise.
 */
extern DECLSPEC int SDLCALL Splat_Prepare(SDL_Window *window);

/** 
 *  Shuts down Splat and frees any unreleased resources.
 *  
 *  The application is responsible for cleaning up the SDL_Window
 *  passed to Splat_Prepare after calling Splat_Finish.
 *  
 *  Returns 0 if successful, 1 otherwise.
 */ 
extern DECLSPEC int SDLCALL Splat_Finish();

/**
 * Creates a Splat image from the given SDL_Surface. 
 *  
 * The application may free the SDL_Surface after the call returns. 
 *  
 * Returns a pointer to a Splat_Image if successful, NULL otherwise.
 */
extern DECLSPEC SDLCALL Splat_Image *Splat_CreateImage(SDL_Surface *surface);

/**
 * Destroys a Splat image previously created. 
 *  
 * This will invalidate any active Splat_Instance objects that still 
 * reference this image.  Those instances must still be destroyed to 
 * free the other resources associated with them.
 *  
 *  Returns 0 if successful, 1 otherwise.
 */
extern DECLSPEC int SDLCALL Splat_DestroyImage(Splat_Image *image);

/**
 * Create a Splat Layer. 
 *  
 * Splat organizes instances of images into overlapping layers.  The 
 * application must create at least one layer to place any image instances. 
 * The new layer will be placed at the top of the stack of layers, but 
 * this can be rearranged with Splat_MoveLayer(). 
 *  
 * Returns a pointer to a new Splat_Layer if successfull, NULL otherwise.
 */
extern DECLSPEC SDLCALL Splat_Layer *Splat_CreateLayer();

/**
 * Destroys a Splat Layer and any associated resources. 
 *  
 * Returns 0 if successful, 1 otherwise. 
 */
extern DECLSPEC int SDLCALL Splat_DestroyLayer(Splat_Layer *layer);

/**
 * Rearranges Splat layer stack. 
 *  
 * Calling this will place 'layer' above 'other' in the layer stack. 
 * If 'layer' is already directly above 'other', the call will succeed 
 * but the stack will remain unchanged. 
 *  
 * Returns 0 if successful, 1 otherwise. 
 */
extern DECLSPEC int SDLCALL Splat_MoveLayer(Splat_Layer *layer, Splat_Layer *other);

/**
 * Create an instance of the given image in the given layer, at the given position.
 * 
 * The instance can be only part of the image.  If subimage is 
 * not NULL, it specifies the bounds of the image to display. 
 *  
 * Returns a pointer to the new Splat_Instance, or NULL if an 
 * error occurs. 
 */
extern DECLSPEC SDLCALL Splat_Instance *Splat_CreateInstance(Splat_Image *image, Splat_Layer *layer, SDL_Point *position, SDL_Rect *subimage);

/**
 * Destroys the specified image instance.
 * 
 * Returns 0 if successful, 1 otherwise. 
 */
extern DECLSPEC int SDLCALL Splat_DestroyInstance(Splat_Instance *instance);

/**
 * Update the position of the image instance.
 * 
 * Returns 0 if successful, 1 otherwise. 
 */
extern DECLSPEC int SDLCALL Splat_SetInstancePosition(Splat_Instance *instance, SDL_Point *position);

/**
 * Move the specified image instance to the specified layer.
 * 
 * Returns 0 if successful, 1 otherwise. 
 */
extern DECLSPEC int SDLCALL Splat_SetInstanceLayer(Splat_Instance *instance, SDL_Layer *layer);

/**
 * Update the image the instance refers. 
 *  
 * If subimage is not NULL, it specifies the bounds of the image 
 * to display.  If image is NULL, only the subimage is updated. 
 * If both are NULL, the instance is updated to the full size of 
 * the image. 
 * 
 * Returns 0 if successful, 1 otherwise. 
 */
extern DECLSPEC int SDLCALL Splat_SetInstanceImage(Splat_Instance *instance, Splat_Image *image, SDL_Rect *subimage);

/**
 * Set the default background color. 
 *  
 * Returns 0 if successful, 1 otherwise. 
 */
extern DECLSPEC int SDLCALL Splat_SetClearColor(SDL_Color *color);

/**
 * Returns the renderer viewport origin, or NULL if an error 
 * occurs. 
 */
extern DECLSPEC SDL SDLCALL_Point *Splat_GetViewPosition();

/**
 * Sets the renderer viewport origin. 
 *  
 * Returns 0 if successful, 1 otherwise. 
 */
extern DECLSPEC int SDLCALL Splat_SetViewPosition(SDL_Point *position);

/**
 * Retrieves the scaling factors for the renderer. 
 *  
 * Returns 0 if sucessful, and the fields pointed to by x and y 
 * are filled with the scaling factors.  Otherwise, the fields 
 * are unmodified and returns 1. 
 */
extern DECLSPEC int SDLCALL Splat_GetScale(float *x, float *y);

/**
 * Set the scaling factors to the specified values. 
 *  
 * Returns 0 if successful, 1 otherwise. 
 */
extern DECLSPEC int SDLCALL Splat_SetScale(float x, float y);

/**
 * Render the scene. 
 *  
 * Returns 0 if successful, 1 otherwise. 
 */
extern DECLSPEC int SDLCALL Splat_Render();

/**
 * Draw a rectangle outline.  Intended primarily for debugging. 
 * Draws above all normal layers. 
 * 
 * @param rect - Bounds of the rectangle.
 * @param color - Color of rectangle.
 * @param width - Width of the rectangle lines in pixels.
 * @param ttl - Number of milliseconds to render the rectangle
 * 
 * @return 0 if successful, 1 otherwise.
 */
extern DECLSPEC int SDLCALL Splat_DrawRect(SDL_Rect *rect, SDL_Color *color, int width, int ttl);

/**
 * Draw a filled rectangle.  Intended primarily for debugging. 
 * Draws above all normal layers. 
 * 
 * @param rect - Bounds of the rectangle.
 * @param color - Color of rectangle.
 * @param ttl - Number of milliseconds to render the rectangle
 * 
 * @return 0 if successful, 1 otherwise.
 */
extern DECLSPEC int SDLCALL Splat_DrawSolidRect(SDL_Rect *rect, SDL_Color *color, int ttl);

/**
 * Draw a line.  Intended primarily for debugging. 
 * Draws above all normal layers. 
 * 
 * @param start - Starting position of the line.
 * @param end - Ending position of the line.
 * @param color - Color of the line.
 * @param width - Width of the line in pixels.
 * @param ttl - Number of miliseconds.
 * 
 * @return 0 if successful, 1 otherwise.
 */ 
extern DECLSPEC void SDLCALL Splat_DrawLine(SDL_Point *start, SDL_Point *end, SDL_Color *color, int width, int ttl);

/**
 * Creates a new canvas to draw on. 
 *  
 * Most applications will create a single canvas, however, it is 
 * possible to create multiple canvases and switch between them.
 * 
 * @return DECLSPEC SDLCALL Splat_Canvas* 
 */
extern DECLSPEC SDLCALL Splat_Canvas *Splat_CreateCanvas();
extern DECLSPEC int SDLCALL Splat_DestroyCanvas(Splat_Canvas *context);
extern DECLSPEC SDLCALL Splat_Canvas *Splat_GetActiveCanvas();
extern DECLSPEC int SDLCALL Splat_SetActiveCanvas(Splat_Canvas *context);

extern DECLSPEC int SDLCALL Splat_LoadVertexShader(const char *source);
extern DECLSPEC int SDLCALL Splat_LoadFragmentShader(const char *source);
extern DECLSPEC int SDLCALL Splat_PrepareShaders();
extern DECLSPEC int SDLCALL Splat_FinishShaders();

extern DECLSPEC int SDLCALL Splat_GetError();

#ifdef __cplusplus
}

namespace Splat {

#define SPLAT_OBJECT_DECL(name) \
protected: \
  class D##name##; \
  unique_ptr<D##name##> d;

class Exception : public std::exception {};
class BadParameterException : public exception {};
class UninitializedException : public exception {};

/**
 *  Prepares Splat for rendering.
 *  
 *  @param window - An SDL window already created by the
 *                application for Splat to use.
 *  
 *  @raises BadParameterException - If window is not a valid
 *          SDL_Window pointer.
 */
void Prepare(SDL_Window *window) throws(Exception);

/** 
 *  Shuts down Splat and frees any unreleased resources.
 *  
 *  The application is responsible for cleaning up the SDL_Window
 *  passed to Splat_Prepare after calling Splat_Finish.
 *  
 *  @raises UninitializedExceptions - If Splat is not properly
 *          initialized by Splat::Prepare()
 */ 
void Finish();

/**
 * Base class for standard Splat classes
 */
class Object {
protected:
  Object();
  ~Object();

  // Prevent copying
  Object(const Object &o);
  operator= Object(const Object &o);
};

/**
 * Image instance class
 */
class Instance : public Object {
  SPLAT_OBJECT_DECL(Instance);

public:
  /**
   * Set the position of the image instance. 
   *  
   * @param position - SDL_Point that specifies the new location 
   *                 in canvas space.
   *  
   * @raises BadParameterException - Thrown if position is 
   *         invalid.
   */
  void SetPosition(SDL_Point *position) throw (Exception);

  /**
   * Get the position of the image instance. 
   *  
   * @return - SDL_Point that specifies the new location 
   *                 in canvas space.
   */
  SDL_Point GetPosition();

  /**
   * Set the layer for this instance.
   * 
   * @param layer - Layer to render this instance 
   *  
   * @raises BadParameterException - Thrown if position is 
   *         invalid.
   */
  void SetLayer(Layer *layer) throw (Exception);
  Layer *GetLayer() throw (Exception);
  void SetImage(Image *image) throw (Exception);
};


class Image : public Object {
  SPLAT_OBJECT_DECL(Image);

public:
  Instance *CreateInstance(Layer *Layer, int x, int y, SDL_Rect *subimage = nullptr);
};

class Layer : public Object {
  SPLAT_OBJECT_DECL(Layer);

public:
  void Move(Layer *other);
};

class Canvas : public Object {
  SPLAT_OBJECT_DECL(Canvas);

public:
  Image *CreateImage(SDL_Surface *surface);
  void DestroyImage(Image *image);

  Layer *CreateLayer();
  void DestroyLayer(Layer *layer);

  void SetClearColor(SDL_Color *color);
  SDL_Point *GetViewPosition();
  SDL_Point *GetViewPosition();
  
  void GetScale(float &x, float &y);
  void SetScale(float x, float y);

  void Render();

  void DrawRect(SDL_Rect *rect, SDL_Color *color, int width = 1, int ttl = 0);
  void DrawSolidRect(SDL_Rect *rect, SDL_Color *color, int ttl = 0);
  void DrawLine(SDL_Point *start, SDL_Point *end, SDL_Color *color, int width = 1, int ttl = 0);
};

}

#endif /* __cplusplus */

#include <SDL2/close_code.h>

