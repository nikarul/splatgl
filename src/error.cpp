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

#include <stdarg.h>
#include <stdio.h>
#include "splat.h"

static const char *error = nullptr;
static char buffer[256];

extern "C"
{

const char *Splat_GetError() {
  return error;
}

void Splat_SetError(const char *errorMsg, ...) {
  if (!errorMsg) {
    error = NULL;
    return;
  }

  va_list args;
  va_start(args, errorMsg);
  vsnprintf(buffer, 256, errorMsg, args);
  va_end(args);

  error = buffer;
}

} // extern "C"

