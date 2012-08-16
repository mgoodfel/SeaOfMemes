/*
  Copyright (C) 1995-2012 by Michael J. Goodfellow

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

#include "GuiTestGL.h"
#include "TextureSurface.h"

// find a font file
BOOL sysFindFont(
  const char* faceName, 
  BOOL bold, 
  BOOL italic, 
  mgString& fontFile);

//--------------------------------------------------------------
// constructor
TextureSurface::TextureSurface(
  GuiTestGL* theApp)
{
  m_theApp = theApp;

  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // this will be used in overlay plane.  no mipmapping
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
}

//--------------------------------------------------------------
// destructor
TextureSurface::~TextureSurface()
{
  if (m_texture != 0)
  {
    glDeleteTextures(1, &m_texture);
    m_texture = 0;
  }
}

//--------------------------------------------------------------
// get texture for normal surfaces (not multi-tiled)
GLuint TextureSurface::getTexture() const
{
  return m_texture;
}

//--------------------------------------------------------------------
// repair damage
void TextureSurface::repair(
  mgRectangle& bounds)
{
  if (isDamaged())
  {
    mgGenSurface::repair(bounds);
    
    int surfaceWidth, surfaceHeight;
    DWORD* surfaceData;
    getPixels(surfaceWidth, surfaceHeight, surfaceData);
    
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // handle special case of entire surface damaged
    if (bounds.m_x == 0 && bounds.m_y == 0 && 
        bounds.m_width == surfaceWidth && bounds.m_height == surfaceHeight)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surfaceWidth, surfaceHeight, 0, 
        GL_BGRA, GL_UNSIGNED_BYTE, surfaceData);
    }
    else
    {
      // unfortunately, OpenGL has no method for copying a rectangle out of memory
      // into a texture.  we have to build an array of the correct size.

      DWORD* data = new DWORD[bounds.m_width * bounds.m_height];
      for (int i = 0; i < bounds.m_height; i++)
      {
        DWORD* dataLine = data + i*bounds.m_width;
        DWORD* surfaceLine = surfaceData + (bounds.m_y + i)*surfaceWidth + bounds.m_x;
        memcpy(dataLine, surfaceLine, bounds.m_width * sizeof(DWORD));
      }

      glTexSubImage2D(GL_TEXTURE_2D, 0, bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height,
                      GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*) data);
      delete data;
    }
  }
}

//--------------------------------------------------------------
// set dimensions of surface
void TextureSurface::setSurfaceSize(
  int width,
  int height)
{
  mgGenSurface::setSurfaceSize(width, height);

  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
}

//--------------------------------------------------------------
// return vertical pixels per inch
int TextureSurface::displayDPI() const
{
  return mgPlatform->getDPI();
}

//--------------------------------------------------------------
// find a font file
BOOL TextureSurface::findFont(
  const char* faceName, 
  BOOL bold, 
  BOOL italic, 
  mgString& fontFile)
{
  return m_theApp->findFont(faceName, bold, italic, fontFile);
}
