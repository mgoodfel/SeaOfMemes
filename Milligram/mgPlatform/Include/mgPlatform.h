/*
  Copyright (C) 1995-2013 by Michael J. Goodfellow

  This source code is distributed for free and may be modified, redistributed, and
  incorporated in other projects (commercial, non-commercial and open-source)
  without restriction.  No attribution to the author is required.  There is
  no requirement to make the source code available (no share-alike required.)

  This source code is distributed "AS IS", with no warranty expressed or implied.
  The user assumes all risks related to quality, accuracy and fitness of use.

  Except where noted, this source code is the sole work of the author, but it has 
  not been checked for any intellectual property infringements such as copyrights, 
  trademarks or patents.  The user assumes all legal risks.  The original version 
  may be found at "http://www.sea-of-memes.com".  The author is not responsible 
  for subsequent alterations.

  Retain this copyright notice and add your own copyrights and revisions above
  this notice.
*/
#ifndef MGPLATFORM_H
#define MGPLATFORM_H

#include "mgUtil/Include/mgUtil.h"

#include "mgApplication.h"
#include "mgInputEvents.h"
#include "mgPlatformErrorTable.h"
#include "mgPlatformServices.h"

#ifdef WIN32
#pragma comment(lib, "mgPlatform.lib")

//#define SUPPORT_DX9                 // define to add DirectX9 support
#define SUPPORT_GL21                // define to add OpenGL 2.1 support
#define SUPPORT_GL33                // define to add OpenGL 3.3 support
#define SUPPORT_FTFONT              // define to use FreeType fonts

#include "Windows/win_glew.h"
#endif

#if defined(__unix__)

#define SUPPORT_GL21                // define to add OpenGL 2.1 support
#define SUPPORT_GL33                // define to add OpenGL 3.3 support
#define SUPPORT_FTFONT              // define to use FreeType fonts

#include "Linux/linux_glew.h"
#endif

#if defined(__APPLE__)
#define SUPPORT_GL21                // define to add OpenGL 2.1 support
#define SUPPORT_GL33                // define to add OpenGL 3.3 support
#define SUPPORT_FTFONT              // define to use FreeType fonts

#include <OpenGL/gl3.h>
#endif

#if defined(EMSCRIPTEN)
#define SUPPORT_WEBGL
//#define SUPPORT_FTFONT              // define to use FreeType fonts
#define SUPPORT_CANVASFONT              // define to use Javascript canvas fonts

// include OpenGL ES headers so we are restricted to WebGL subset
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#endif

#endif
