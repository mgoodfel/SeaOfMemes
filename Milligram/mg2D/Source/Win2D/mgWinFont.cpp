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

#include "mgXPFont.h"
#include "mgXPSurface.h"

//--------------------------------------------------------------
// constructor
mgXPFont::mgXPFont(
  mgXPSurface* surface,
  const mgString& faceName,
  int size,
  BOOL bold,
  BOOL italic)
{
  int pixelSize = surface->points(size);

  WCHAR* wideFace; int wideLen;
  faceName.toWCHAR(wideFace, wideLen);

  // create new font
  m_platformFont = CreateFont(
     -pixelSize,      // height of font (cell size)
     0,               // average character width
     0,               // angle of escapement
     0,               // base-line orientation angle
     bold ? FW_BOLD : FW_NORMAL,       // font weight
     italic,           // italic attribute option
     false,           // underline attribute option
     false,           // strikeout attribute option
     ANSI_CHARSET,   // character set identifier
     OUT_DEFAULT_PRECIS,  // output precision
     CLIP_DEFAULT_PRECIS, // clipping precision
     CLEARTYPE_QUALITY,      // output quality  
     DEFAULT_PITCH ,   // pitch and family
     wideFace          // typeface name
  );

  delete wideFace;

  // get font and ascent from metrics
  SelectObject(surface->m_surfaceDC, m_platformFont);
  TEXTMETRIC metrics;
  GetTextMetrics(surface->m_surfaceDC, &metrics);
  SelectObject(surface->m_surfaceDC, (HFONT) NULL);

  m_faceName = faceName;
  m_size = size;
  m_bold = bold;
  m_italic = italic;
  m_height = metrics.tmHeight;
  m_ascent = metrics.tmAscent;
  m_charWidth = metrics.tmAveCharWidth;
}

//--------------------------------------------------------------
// destructor
mgXPFont::~mgXPFont()
{
  if (m_platformFont != NULL)
  {
    DeleteObject(m_platformFont);
    m_platformFont = NULL;
  }
}

