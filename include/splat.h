/*
  Splat Graphics Library C API
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

/**
 * @file csplat.h
 *
 * Main include header for the Splat C API
 */

#include <stdint.h>
#include <SDL2/begin_code.h>
typedef struct SDL_Window SDL_Window;
typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Point SDL_Point;
typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Color SDL_Color;
typedef struct Splat_Image Splat_Image;
typedef struct Splat_Layer Splat_Layer;
typedef struct Splat_Instance Splat_Instance;
typedef struct Splat_Canvas Splat_Canvas;
typedef struct Splat_Shader Splat_Shader;
typedef struct Splat_Program Splat_Program;

typedef enum {
  SPLAT_MIRROR_X = 0x0001, // Only applies to images
  SPLAT_MIRROR_Y = 0x0002, // Only applies to images
  SPLAT_MIRROR_DIAG = 0x0004, // Only applies to images
  SPLAT_RELATIVE = 0x0008, // Applies to images, debug rects/lines
  SPLAT_ROTATE = 0x0010, // Only applies to images
  SPLAT_STATIC = 0x0020, // Only applies to images
  SPLAT_FILLED = 0x0020, // Only applies to debug rects
} Splat_Flags;

typedef enum {
  SPLAT_VERTEX_SHADER = 0,
  SPLAT_FRAGMENT_SHADER,
  SPLAT_GEOMETRY_SHADER,
} Splat_ShaderType;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *  Prepares Splat for rendering.
 *
 *  The window argument is an SDL window already created by the
 *  application for Splat to use.
 *
 *  Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_Prepare(SDL_Window *window, int viewportWidth, int viewportHeight);

/**
 *  Shuts down Splat and frees any unreleased resources.
 *
 *  The application is responsible for cleaning up the SDL_Window
 *  passed to Splat_Prepare after calling Splat_Finish.
 */
DECLSPEC void SDLCALL Splat_Finish();

/**
 * Creates a Splat image from the given SDL_Surface.
 *
 * The application may free the SDL_Surface after the call returns.
 *
 * Returns a pointer to a Splat_Image if successful, NULL otherwise.
 */
DECLSPEC SDLCALL Splat_Image *Splat_CreateImage(SDL_Surface *surface);

/**
 * Destroys a Splat image previously created.
 *
 * This will invalidate any active Splat_Instance objects that still
 * reference this image.  Those instances must still be destroyed to
 * free the other resources associated with them.
 *
 *  Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_DestroyImage(Splat_Image *image);

/**
 * Retrieves the given images width and height.
 *
 * @param image Image to size.
 * @param width Pointer to uint32_t that will be filled with the image's width.
 * @param height Pointer to uint32_t that will be filled with the image's width.
 *
 * @return 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_GetImageSize(Splat_Image *image, uint32_t *width, uint32_t *height);

/**
 * Create a Splat Layer.
 *
 * Splat organizes instances of images into overlapping layers.  The
 * application must create at least one layer to place any image instances.
 * The new layer will be placed at the bottom of the stack of layers, but
 * this can be rearranged with Splat_MoveLayer().
 *
 * @param canvas Canvas in which to create layer
 *
 * Returns a pointer to a new Splat_Layer if successfull, NULL otherwise.
 */
DECLSPEC SDLCALL Splat_Layer *Splat_CreateLayer(Splat_Canvas *canvas);

/**
 * Destroys a Splat Layer and any associated resources.
 *
 * Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_DestroyLayer(Splat_Layer *layer);

/**
 * Rearranges Splat layer stack.
 *
 * Calling this will place 'layer' above 'other' in the layer stack.
 * If 'layer' is already directly above 'other', the call will succeed
 * but the stack will remain unchanged.  If 'other' is NULL, 'layer'
 * will be placed at the bottom of the layer stack.
 *
 * Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_MoveLayer(Splat_Layer *layer, Splat_Layer *other);

/**
 * Create an instance of the given image in the given layer, at the given position.
 *
 * The instance can be only part of the image.  If subimage is
 * not NULL, it specifies the bounds of the image to display.
 *
 * Returns a pointer to the new Splat_Instance, or NULL if an
 * error occurs.
 */
DECLSPEC Splat_Instance *SDLCALL Splat_CreateInstance(Splat_Image *image, Splat_Layer *layer, int x, int y, float s1, float t1, float s2, float t2, uint32_t flags);

/**
 * Destroys the specified image instance.
 *
 * Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_DestroyInstance(Splat_Instance *instance);

/**
 * Update the position of the image instance.
 *
 * Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_SetInstancePosition(Splat_Instance *instance, int x, int y);

/**
 * Move the specified image instance to the specified layer.
 *
 * Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_SetInstanceLayer(Splat_Instance *instance, Splat_Layer *layer);

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
DECLSPEC int SDLCALL Splat_SetInstanceImage(Splat_Instance *instance, Splat_Image *image, float s1, float t1, float s2, float t2);

/**
 * Set the default background color.
 *
 * Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_SetClearColor(Splat_Canvas *canvas, float r, float g, float b, float a);

/**
 * Returns the renderer viewport origin, or NULL if an error
 * occurs.
 */
DECLSPEC int SDLCALL Splat_GetViewPosition(Splat_Canvas *canvas, SDL_Point *position);

/**
 * Sets the renderer viewport origin.
 *
 * Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_SetViewPosition(Splat_Canvas *canvas, SDL_Point *position);

/**
 * Retrieves the scaling factors for the renderer.
 *
 * Returns 0 if sucessful, and the fields pointed to by x and y
 * are filled with the scaling factors.  Otherwise, the fields
 * are unmodified and returns 1.
 */
DECLSPEC int SDLCALL Splat_GetScale(Splat_Canvas *canvas, float *x, float *y);

/**
 * Set the scaling factors to the specified values.
 *
 * Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_SetScale(Splat_Canvas *canvas, float x, float y);

/**
 * Render the scene.
 *
 * Returns 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_Render(Splat_Canvas *canvas);

/**
 * Draw a rectangle outline.  Intended primarily for debugging.
 * Draws above all normal layers.
 *
 * @param rect - Bounds of the rectangle.
 * @param color - Color of rectangle.
 * @param width - Width of the rectangle lines in pixels.
 * @param flags - Flags for rect, only valid flags are
 *      	SPLAT_FILLED, and SPLAT_RELATIVE, rest are ignored.
 * @param ttl - Number of milliseconds to render the rectangle.
 *
 * @return 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_DrawRect(Splat_Canvas *canvas, SDL_Rect *rect, SDL_Color *color, int width, int flags, int ttl);

/**
 * Draw a line.  Intended primarily for debugging.
 * Draws above all normal layers.
 *
 * @param start - Starting position of the line.
 * @param end - Ending position of the line.
 * @param color - Color of the line.
 * @param width - Width of the line in pixels.
 * @param flags - Flags for rect, only valid flag
 *      	is SPLAT_RELATIVE, rest are ignored.
 * @param ttl - Number of milliseconds to render
 *            the line.
 *
 * @return 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_DrawLine(Splat_Canvas *canvas, SDL_Point *start, SDL_Point *end, SDL_Color *color, int width, int flags, int ttl);

/**
 * Creates a new canvas to draw on.
 *
 * Most applications will create a single canvas, however, it is
 * possible to create multiple canvases and switch between them.
 *
 * @return DECLSPEC SDLCALL Splat_Canvas*
 */
DECLSPEC SDLCALL Splat_Canvas *Splat_CreateCanvas();

/**
 * Destroys the given canvas to draw on.  All images, layers,
 * and instances associated with it will be invalidated and
 * destroyed.
 *
 * @param canvas - Canvas to destroy.
 *
 * @return 0 if successful, 1 otherwise.
 */
DECLSPEC int SDLCALL Splat_DestroyCanvas(Splat_Canvas *canvas);

/**
 * Retrieve the current error string, if any.
 *
 * @return The current error string, or NULL if no error has occurred.
 */
DECLSPEC const char *SDLCALL Splat_GetError();

/**
 * Sets the current error string
 *
 * @param error - The error string to set, or NULL to clear the error string.
 */
DECLSPEC void SDLCALL Splat_SetError(const char *error, ...);

/**
 * Convenience macro to clear the current Splat error string
 */
#define Splat_ClearError() Splat_SetError(0)

//TODO SPLAT_SHADERS_EXPERIMENTAL
//DECLSPEC Splat_Shader *SDLCALL Splat_CreateShader(const char *source, int shaderType);
//DECLSPEC int SDLCALL Splat_DestroyShader(Splat_Shader *shader);
//
//DECLSPEC Splat_Program *SDLCALL Splat_CreateProgram();
//DECLSPEC int SDLCALL Splat_DestroyProgram(Splat_Program *program);
//DECLSPEC int SDLCALL Splat_AttachShader(Splat_Program *program, Splat_Shader *shader);
//DECLSPEC int SDLCALL Splat_LinkProgram(Splat_Program *program);
//
//DECLSPEC int SDLCALL Splat_SetCanvasProgram(Splat_Canvas *canvas, Splat_Program *program);
//
//DECLSPEC int SDLCALL Splat_SetImageDefaultProgram(Splat_Image *image, Splat_Program *program);
//DECLSPEC int SDLCALL Splat_SetInstanceProgram(Splat_Instance *instance, Splat_Program *program);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <SDL2/close_code.h>

