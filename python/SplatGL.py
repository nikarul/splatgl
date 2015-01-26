# Splat Graphics Library C API
# Copyright (C) 2014  Michael Dale Long <mlong@digitalbytes.net>
# http://digitalbytes.net/projects/splatgl/
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

from ctypes import CDLL, c_char_p, c_uint32, c_int, c_float, Structure, POINTER, byref
from ctypes.util import find_library
from sdl2 import SDL_Rect, SDL_Point, SDL_Surface, SDL_Window, SDL_Color
from enum import IntEnum

class error(Exception):
	def __init__(self):
		error = _get_error()
		_set_error(None)
		if error is not None:
			super().__init__(error.decode())
		else:
			super().__init__("Undefined Splat error")

def _validate_ptr(result, func, arguments):
	#print("{}{} => {}".format(func.name, arguments, result))
	assert result is not None
	if result == 0:
		raise error()
	else:
		return result

def _validate_int(result, func, arguments):
	#print("{}{} => {}".format(func.name, arguments, result))
	if result != 0:
		raise error()
	else:
		return 0

_dllname = find_library("splatgl")
_libsplatgl = CDLL(_dllname)

def _bind(name, argtypes=None, restype=None, errcheck=None):
	func = getattr(_libsplatgl, name)
	if func is None:
		raise RuntimeException("Failed to bind to function '{}' in libsplatgl.so".format(name))

	func.argtypes = argtypes
	func.restype = restype
	if errcheck is not None:
		func.errcheck = errcheck

	return func

class Splat_Image(Structure):
	pass

class Splat_Layer(Structure):
	pass

class Splat_Instance(Structure):
	pass

class Splat_Canvas(Structure):
	pass

class Flags(IntEnum):
    MIRROR_X = 0x0001
    MIRROR_Y = 0x0002
    MIRROR_DIAG = 0x0004
    RELATIVE = 0x0008
    ROTATE = 0x0010
    STATIC = 0x0020
    FILLED = 0x0020

prepare = _bind("Splat_Prepare", [POINTER(SDL_Window), c_int, c_int], c_int, _validate_int)
finish = _bind("Splat_Finish")
create_image = _bind("Splat_CreateImage", [POINTER(SDL_Surface)], POINTER(Splat_Image), _validate_ptr)
update_image = _bind("Splat_UpdateImage", [POINTER(Splat_Image), POINTER(SDL_Surface)], c_int, _validate_int)
destroy_image = _bind("Splat_DestroyImage", [POINTER(Splat_Image)], c_int, _validate_int)
create_layer = _bind("Splat_CreateLayer", [POINTER(Splat_Canvas)], POINTER(Splat_Layer), _validate_ptr)
destroy_layer = _bind("Splat_DestroyLayer", [POINTER(Splat_Layer)], c_int, _validate_int)
move_layer = _bind("Splat_MoveLayer", [POINTER(Splat_Layer)], c_int, _validate_int)
create_instance = _bind("Splat_CreateInstance", [POINTER(Splat_Image), POINTER(Splat_Layer), c_int, c_int, c_float, c_float, c_float, c_float, c_uint32], POINTER(Splat_Instance), _validate_ptr)
destroy_instance = _bind("Splat_DestroyInstance", [POINTER(Splat_Instance)], c_int, _validate_int)
set_instance_position = _bind("Splat_SetInstancePosition", [POINTER(Splat_Instance), c_int, c_int], c_int, _validate_int)
set_instance_layer = _bind("Splat_SetInstanceLayer", [POINTER(Splat_Instance), POINTER(Splat_Layer)], c_int, _validate_int)
set_instance_image = _bind("Splat_SetInstanceImage", [POINTER(Splat_Instance), POINTER(Splat_Image), c_float, c_float, c_float, c_float], c_int, _validate_int)
set_instance_flags = _bind("Splat_SetInstanceFlags", [POINTER(Splat_Instance), c_uint32], c_int, _validate_int)
set_clear_color = _bind("Splat_SetClearColor", [POINTER(Splat_Canvas), c_float, c_float, c_float, c_float], c_int, _validate_int)
_get_view_position = _bind("Splat_GetViewPosition", [POINTER(Splat_Canvas), POINTER(SDL_Point)], c_int, _validate_int)
set_view_position =  _bind("Splat_SetViewPosition", [POINTER(Splat_Canvas), POINTER(SDL_Point)], c_int, _validate_int)
_get_scale = _bind("Splat_GetScale", [POINTER(Splat_Canvas), POINTER(c_float), POINTER(c_float)], c_int, _validate_int)
set_scale = _bind("Splat_SetScale", [POINTER(Splat_Canvas), c_float, c_float], c_int, _validate_int)
render = _bind("Splat_Render", [POINTER(Splat_Canvas)], c_int, _validate_int)

create_canvas = _bind("Splat_CreateCanvas", None, POINTER(Splat_Canvas), _validate_ptr)
destroy_canvas = _bind("Splat_DestroyCanvas", [POINTER(Splat_Canvas)], c_int, _validate_int)

draw_rect = _bind("Splat_DrawRect", [POINTER(Splat_Canvas), POINTER(SDL_Rect), POINTER(SDL_Color), c_int, c_int, c_int], c_int, _validate_int)
draw_line = _bind("Splat_DrawLine", [POINTER(Splat_Canvas), POINTER(SDL_Point), POINTER(SDL_Point), POINTER(SDL_Color), c_int, c_int, c_int], c_int, _validate_int)

_get_error = _bind("Splat_GetError", None, c_char_p)
_set_error = _bind("Splat_SetError", [c_char_p])

_get_image_size = _bind("Splat_GetImageSize", [POINTER(Splat_Image), POINTER(c_uint32), POINTER(c_uint32)], c_int, _validate_int)

def get_view_position(canvas):
	pt = SDL_Point()
	_get_view_position(canvas, byref(pt))
	return pt

def get_scale(canvas):
	x = c_float()
	y = c_float()
	_get_scale(canvas, byref(x), byref(y))
	return x.value, y.value

def get_image_size(image):
	x = c_uint32()
	y = c_uint32()
	_get_image_size(image, byref(x), byref(y))
	return x.value, y.value

