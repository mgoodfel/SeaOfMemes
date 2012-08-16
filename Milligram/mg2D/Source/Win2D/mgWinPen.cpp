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
#ifdef SUPPORT_WINXP_GRAPHICS

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgXPPen.h"

//--------------------------------------------------------------
// constructor
mgXPPen::mgXPPen(
  int r,
  int g,
  int b,
  int thick)
{
  m_platformPen = CreatePen(PS_SOLID, thick, RGB(r, g, b));
}

//--------------------------------------------------------------
// destructor
mgXPPen::~mgXPPen()
{
  if (m_platformPen != NULL)
  {
    DeleteObject(m_platformPen);
    m_platformPen = NULL;
  }
}

#endif
