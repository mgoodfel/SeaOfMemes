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

#include "Surfaces/mgGenFont.h"
#include "Surfaces/mgGenSurface.h"

//--------------------------------------------------------------
// constructor
mgGenFont::mgGenFont(
  mgGenSurface* surface,
  const char* faceName,
  int size,
  BOOL bold,
  BOOL italic)
{
  m_surface = surface;
  int pixelSize = m_surface->points(size);

  m_faceName = faceName;
  m_size = size;
  m_bold = bold;
  m_italic = italic;

  m_surface->loadFont(this);
}

//--------------------------------------------------------------
// destructor
mgGenFont::~mgGenFont()
{
  // delete font from handle
  m_surface->deleteFont(this);
}

//--------------------------------------------------------------
// get box around a string
void mgGenFont::stringExtent(
  const char* text,
  int textLen,
  mgPoint& endPt,
  mgRectangle& bounds) const
{
  m_surface->stringExtent(this, text, textLen, endPt, bounds);
}

//--------------------------------------------------------------
// get width of string
int mgGenFont::stringWidth(
  const char* text,
  int textLen) const
{
  return m_surface->stringWidth(this, text, textLen);
}

//--------------------------------------------------------------
// get number of chars which fit in width
int mgGenFont::stringFit(
  const char* text,
  int textLen,
  int width) const
{
  return m_surface->stringFit(this, text, textLen, width);
}
