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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgPlatformErrorTable.h"

//--------------------------------------------------------------
// constructor
mgPlatformErrorTable::mgPlatformErrorTable()
{
  mgString xml;
  xml += "<errorTable>";

  xml += "<errorMsg id=\"winBadRegister\">Windows: Failed to register window class.</errorMsg>";

  xml += "<errorMsg id=\"winUnregister\">Windows: Failed to unregister window class.</errorMsg>";

  xml += "<errorMsg id=\"winBadCreate\">Windows: Unable to create window.</errorMsg>";

  xml += "<errorMsg id=\"winBadDC\">Windows: Unable to create device context.</errorMsg>";

  xml += "<errorMsg id=\"winNoLibrary\">Unable to initialize any of <var name=\"libraries\"/>.</errorMsg>";

  xml += "<errorMsg id=\"winBadLibrary\">Unable to initialize <var name=\"library\"/>.</errorMsg>";

  xml += "<errorMsg id=\"winWrongLibrary\">Library \"<var name=\"library\"/>\" not supported.  Try one of <var name=\"libraries\"/>.</errorMsg>";

  xml += "<errorMsg id=\"winDisplayMode\">Cannot find current display mode: <var name=\"width\"/> by <var name=\"height\"/> ";
  xml += "by <var name=\"depth\"/>, <var name=\"freq\"/> hz, flags=<var name=\"flags\"/>.</errorMsg>";

  xml += "<errorMsg id=\"winFullscreen\">Fullscreen mode failed.</errorMsg>";

  xml += "<errorMsg id=\"winNoDisplay\">Display not initialized by application, or initialization failed.</errorMsg>";

  xml += "<errorMsg id=\"linuxNoConfig\">Failed to retrieve a framebuffer config.</errorMsg>";

  xml += "<errorMsg id=\"linuxNullConfig\">glXGetVisualFromFBConfig returned null.</errorMsg>";

  xml += "<errorMsg id=\"linuxBadCreate\">Unable to create window.</errorMsg>";

  xml += "<errorMsg id=\"linuxBadDisplay\">Unable to connect to display.</errorMsg>";

  xml += "<errorMsg id=\"linuxNoDisplay\">Display not initialized by application, or initialization failed.</errorMsg>";

  xml += "<errorMsg id=\"glChoosePixelFormat\">mgGLInit: cannot ChoosePixelFormat.</errorMsg>";

  xml += "<errorMsg id=\"glSetPixelFormat\">mgGLInit: cannot SetPixelFormat.</errorMsg>";

  xml += "<errorMsg id=\"glCreateContext\">mgGLInit: wglCreateContext failed.</errorMsg>";

  xml += "<errorMsg id=\"glMakeCurrent\">mgGLInit: wglMakeCurrent failed.</errorMsg>";

  xml += "</errorTable>";

  parse(xml.length(), xml);
}

