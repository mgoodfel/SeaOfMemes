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

#include "mgGenFont.h"
#include "mgGenCharCache.h"
#include "Graphics2D/Surfaces/mgGenSurface.h"

#include "ft2build.h"
#include FT_FREETYPE_H

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

  mgString fontFile;
  if (!m_surface->findFont(m_faceName, m_bold, m_italic, fontFile))
  {
    if (!m_surface->findFont("default", m_bold, m_italic, fontFile))
     throw new mgException("cannot find font %s%s%s or default.", 
        (const char*) m_faceName, m_bold?"-b":"", m_italic?"-i":"");
  }

  // =-= on errors, try default font face
  FT_Face face;
  int error = FT_New_Face((FT_Library) m_surface->m_ftLibrary, fontFile, 0, &face); 
  if (error == FT_Err_Unknown_File_Format) 
  { 
    throw new mgException("unknown file format %s", (const char*) fontFile);
  } 
  else if (error != 0)
  { 
    throw new mgException("FT_New_Face(%s) returns %d", (const char*) fontFile, error);
  }

  mgDebug("load font %s - %s : %d faces, %d glyphs.", face->family_name, face->style_name, face->num_faces, face->num_glyphs);

  int dpi = surface->displayDPI();
  error = FT_Set_Char_Size(face, 0, m_size*64, dpi, dpi);
  if (error != 0)
    mgDebug("FT_Set_Char_Size = %d", error);

  m_ftFace = face;

  m_height = face->size->metrics.height/64;
  m_ascent = face->size->metrics.ascender/64;

  const mgGenCharDefn* defn = m_surface->m_charCache->getChar(face, 'n');
  if (defn != NULL)
    m_charWidth = defn->m_advanceX;
  else mgDebug("No 'n' width in font");
}

//--------------------------------------------------------------
// destructor
mgGenFont::~mgGenFont()
{
  if (m_ftFace != NULL)
  {
    FT_Done_Face((FT_Face) m_ftFace);
    m_ftFace = NULL;
  }
}

//--------------------------------------------------------------
// get box around a string
void mgGenFont::stringExtent(
  const char* text,
  int textLen,
  mgPoint& endPt,
  mgRectangle& bounds) const
{
  int x = 0;
  int y = 0;

  int left = INT_MAX;
  int top = INT_MAX;
  int right = INT_MIN;
  int bottom = INT_MIN;

  int textPosn = 0;
  while (textPosn < textLen)
  {
    int utfCount;
    int letter = mgString::fromUTF8(text+textPosn, utfCount);
    textPosn += utfCount;

    const mgGenCharDefn* defn = m_surface->m_charCache->getChar(m_ftFace, letter);
    if (defn == NULL)
      continue;

    left = min(left, x + defn->m_bitmapX);
    right = max(right, x + defn->m_bitmapX + defn->m_bitmapWidth);

    top = min(top, y - defn->m_bitmapY);
    bottom = max(bottom, y - defn->m_bitmapY + defn->m_bitmapHeight);

    x += defn->m_advanceX;
  }

  // round end point up, in case something positioned there
  endPt.m_x = x;
  endPt.m_y = y;

  // round bounding box min down and size up, so text can't spill over
  bounds.m_x = left;
  bounds.m_y = top;
  bounds.m_width = right - left;
  bounds.m_height = bottom - top;
}

//--------------------------------------------------------------
// get width of string
int mgGenFont::stringWidth(
  const char* text,
  int textLen) const
{
  int x = 0;
  int y = 0;

  int textPosn = 0;
  while (textPosn < textLen)
  {
    int utfCount;
    int letter = mgString::fromUTF8(text+textPosn, utfCount);
    textPosn += utfCount;

    const mgGenCharDefn* defn = m_surface->m_charCache->getChar(m_ftFace, letter);
    if (defn == NULL)
      continue;

    x += defn->m_advanceX;
  }

  return x;
}

//--------------------------------------------------------------
// get number of chars which fit in width
int mgGenFont::stringFit(
  const char* text,
  int textLen,
  int width) const
{
  int x = 0;
  int y = 0;

  int textPosn = 0;
  while (textPosn < textLen)
  {
    int utfCount;
    int letter = mgString::fromUTF8(text+textPosn, utfCount);
    textPosn += utfCount;

    const mgGenCharDefn* defn = m_surface->m_charCache->getChar(m_ftFace, letter);
    if (defn == NULL)
      continue;

    x += defn->m_advanceX;
    if (x > width)
      return textPosn - utfCount;  // prev char was the last that fit
  }

  return textPosn;  // complete string fit
}
