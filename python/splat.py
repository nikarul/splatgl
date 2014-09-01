# Splat 2D Rendering Library C API
# Copyright (C) 2014  Michael Dale Long <mlong@digitalbytes.net>
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

from ctypes import CDLL

_libsplat = CDLL("libsplat.so") ###TODO support other OSes/sonames

FL_MIRROR_X = 0x01
FL_MIRROR_Y = 0x02
FL_MIRROR_DIAG = 0x04
FL_RELATIVE = 0x08
FL_ROTATE = 0x10
FL_STATIC = 0x20

# Map all Splat_* symbols into module namespace, sans Splat_ prefix
Prepare = _libsplat.Splat_Prepare
Finish = _libsplat.Splat_Finish
CreateImage = _libsplat.Splat_CreateImage
DestroyImage = _libsplat.Splat_DestroyImage
CreateLayer = _libsplat.Splat_CreateLayer
DestroyLayer = _libsplat.Splat_DestroyLayer
MoveLayer = _libsplat.Splat_MoveLayer
CreateInstance = _libsplat.Splat_CreateInstance
DestroyInstance = _libsplat.Splat_DestroyInstance
SetInstancePosition = _libsplat.Splat_SetInstancePosition
SetInstanceLayer = _libsplat.Splat_SetInstanceLayer
SetInstanceImage = _libsplat.Splat_SetInstanceImage
SetClearColor = _libsplat.Splat_SetClearColor
GetViewPosition = _libsplat.Splat_GetViewPosition
SetViewPosition = _libsplat.Splat_SetViewPosition
GetScale = _libsplat.Splat_GetScale
SetScale = _libsplat.Splat_SetScale
Render = _libsplat.Splat_Render
CreateCanvas = _libsplat.Splat_CreateCanvas
DestroyCanvas = _libsplat.Splat_DestroyCanvas
GetError = _libsplat.Splat_GetError
SetError = _libsplat.Splat_SetError
GetCanvas = _libsplat.Splat_GetCanvas
SetCanvas = _libsplat.Splat_SetCanvas

def DrawRect(*args):
    pass ###TODO

def DrawLine(*args):
    pass ###TODO

