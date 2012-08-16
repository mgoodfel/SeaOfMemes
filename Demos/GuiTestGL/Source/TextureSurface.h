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
#ifndef TEXTURESURFACE_H
#define TEXTURESURFACE_H

#include "mg2D/Include/Graphics2D/Surfaces/mgGenSurface.h"

class GuiTestGL;

// interface to texture for a surface
class TextureSurface : public mgGenSurface
{
public:
  // constructor
  TextureSurface(
    GuiTestGL* theApp);

  // destructor
  virtual ~TextureSurface();

  // return texture containing surface data
  virtual GLuint getTexture() const;

  // repair damage
  virtual void repair(
    mgRectangle& bounds);

  // set dimensions of surface
  virtual void setSurfaceSize(
    int width,
    int height);

protected:
  GLuint m_texture;
  GuiTestGL* m_theApp;

  // return vertical pixels per inch
  virtual int displayDPI() const;

  // find a font file
  virtual BOOL findFont(
    const char* faceName, 
    BOOL bold, 
    BOOL italic, 
    mgString& fontFile);
};

#endif

