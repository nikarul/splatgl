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

#ifndef __LIB_SPLAT_H__
#define __LIB_SPLAT_H__

#include <GL/gl.h>
#include <array>
#include <cstdint>
#include <exception>
#include <memory>
#include <SDL.h>

/**
 * @file splat.h 
 *  
 * Main include header for Splat
 */

/**
 *  @mainpage Splat 2D Rendering Library
 *  
 *  http://www.digitalbytes.net/libsplat/
 *  
 *  @section intro_sec Introduction
 *  
 *  This is Splat, an SDL-based 2D sprite/tile rendering library.
 *  
 *  Splat is written in C++, but provides a C API as well.
 *  
 *  This library is distributed under Zlib License, found in the file "LICENSE".
 *  
 */

namespace Splat {

#define SPLAT_OBJECT_DECL(name) \
protected: \
  class D##name;
  std::unique_ptr<D##name> d;  
  
#ifdef __WIN32__
  #define SPLAT_PUBLIC	__declspec(dllexport)
  #define SPLAT_LOCAL
  #ifdef __GNUC__
    #define SPLAT_INLINE inline
  #else
    #define SPLAT_INLINE __force_inline
  #endif
#else
  #define SPLAT_PUBLIC	__attribute__ ((visibility("default")))
  #define SPLAT_LOCAL	__attribute__ ((visibility("hidden")))
  #define SPLAT_INLINE inline
#endif

class SPLAT_PUBLIC Exception : public std::exception {};
class SPLAT_PUBLIC BadParameterException : public Exception {};
class SPLAT_PUBLIC WrongCanvasException : public Exception {};
class SPLAT_PUBLIC DriverException : public Exception {
public:
  DriverException(const char *error);
};

class SPLAT_PUBLIC OpenGLException : public DriverException {
public:
  GLenum error;

  OpenGLException(GLenum e) : DriverException("glGetErrorString(e)"), error(e) {}
};


//class DCanvas;
//class DImage;
//class DInstance;
//class DLayer;

class Canvas;

typedef std::array<float, 2> scale_t;
typedef std::array<uint32_t, 2> extents_t;
typedef std::array<float, 4> color_t;

/**
 * Base class for standard Splat classes
 */
class SPLAT_PUBLIC Object {
protected:
  Object();
  ~Object();

  // Prevent copying
  Object(const Object &o);
  Object &operator=(const Object &o);
};

/**
 * Layer class
 */
class SPLAT_PUBLIC Layer : public Object {
  SPLAT_OBJECT_DECL(Layer);
  
public:
  /**
   * @return SDL_Point - The offset all objects in this layer.
   */
  SDL_Point GetOffset();

  /**
   * @param offset - Offset to apply to all objects in this layer.
   */
  void SetOffset(SDL_Point *offset);

  /**
   * @return scale_t - The X/Y scale applied to this layer
   */
  scale_t GetScale();

  /**
   * Set the X/Y scale applied to this layer
   * 
   * @param x - X scale factor.  1.0 means no scale (default). 
   * @param y - X scale factor.  1.0 means no scale (default). 
   */
  void SetScale(float x, float y);
};

/**
 * Image class
 */
class SPLAT_PUBLIC Image : public Object {
  SPLAT_OBJECT_DECL(Image);

public:
  /**
   * Create an instance of this image on the image's canvas.
   * 
   * @param layer - Layer at which to render.
   * @param position - Position at which to render.
   * @param subimage - Pointer to SDL_Rect that specifies the 
   *                 subimage to use in pixels.  If null, assume
   *                 the full bounds of the image.
   * 
   * @return Instance * - New instance handle.
   */
  Instance *CreateInstance(int x, int y, Layer *layer, SDL_Rect *subimage = nullptr);
  void DestroyInstance(Instance *instance);

  void Update(SDL_Surface *surface);
  extents_t GetExtents();
};

/**
 * Image instance class
 */
class SPLAT_PUBLIC Instance : public Object {
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
  void SetPosition(SDL_Point *position);

  /**
   * Get the position of the image instance. 
   *  
   * @return - SDL_Point that specifies the new location 
   *                 in canvas space.
   */
  SDL_Point GetPosition();

  SDL_Rect GetClipRect();
  void SetClipRect(SDL_Rect *rect);
  void SetColor(color_t& color);
  void SetAngle(float angle);

  /**
   * @return scale_t - The X/Y scale applied to this image instance
   */
  scale_t GetScale();

  /**
   * Set the X/Y scale applied to this image instance
   * 
   * @param x - X scale factor.  1.0 means no scale (default). 
   * @param y - X scale factor.  1.0 means no scale (default). 
   */
  void SetScale(float x, float y);

  bool IsFixed();
  void SetFixed(bool fixed);

  bool IsVisible();
  void SetVisible(bool visible);

  /**
   * Set the layer for this instance.
   * 
   * @param layer - Layer at which to render this instance 
   */
  void SetLayer(Layer *layer);

  /**
   * Get the layer for this instance.
   * 
   * @return Layer in which this instance renders
   */
  Layer *GetLayer();

  /**
   * Set the image the instance refers. 
   *  
   * If subimage is not NULL, it specifies the bounds of the image 
   * to display.  If image is NULL, only the subimage is updated. 
   * If both are NULL, the instance is updated to the full size of 
   * the image. 
   * 
   * @param image - Image to use for rendering.  If this is null, 
   *              keep the existing image, just update the
   *              subimage.
   * @param subimage - Pointer to SDL_Rect that specifies the 
   *                 subimage to use in pixels.  If null, assume
   *                 the full bounds of the image.
   *  
   * @raises BadParameterException - If image or subimage is 
   *         invalid.
   * @raises WrongCanvasException - If the image does belong to 
   *         the same canvas as this instance.
   */
  void SetImage(Image *image, SDL_Rect *subimage = nullptr);
};

/**
 * Canvas class
 */
class SPLAT_PUBLIC Canvas : public Object {
  SPLAT_OBJECT_DECL(Canvas);

public:
  /**
   * Construct a Canvas object.
   *  
   * Multiple canvases can point to the same window to allow switching 
   * between scenes or contexts.  Note that it only makes sense to render 
   * one canvas at a time. 
   *  
   * You can also render to multiple windows simultaneously from different 
   * canvases, if the underlying driver implementation supports that. 
   *  
   * @param window - Points to an SDL Window initialized as desired for the canvas to use. 
   *  
   * @raise BadParameterException - If the window is invalid. 
   * @raise DriverException - If an driver error occurs. 
   */
  Canvas(SDL_Window *window);

  /**
   * Canvas Destructor.  Cleans up the OpenGL set up.  It is the application's 
   * responsibility to destroy the SDL window and clean up SDL afterwards.
   */
  ~Canvas();

  Image *CreateImage(SDL_Surface *surface);
  void DestroyImage(Image *image);

  void SetClearColor(color_t &color);

  Layer *CreateLayer();
  Layer *CreateLayer(Layer *upper);
  void MoveLayerToTop(Layer *layer);
  void MoveLayer(Layer *layer, Layer *upper);
  void DestroyLayer(Layer *layer);

  void Render();

  void DrawDebugRect(SDL_Rect *rect, color_t &color, unsigned int width = 1, unsigned int ttl = 0, bool filled = false, bool relative = true);
  void DrawDebugLine(SDL_Point *start, SDL_Point *end, color_t &color, unsigned int width = 1, unsigned int ttl = 0, bool relative = true);
};

}

#endif // #ifndef __LIB_SPLAT_H__

