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

#include "GUI/mgControl.h"
#include "GUI/mgSolidPaint.h"

//--------------------------------------------------------------
// constructor
const mgSolidPaint* mgSolidPaint::createPaint(
  mgSurface* surface,
  const mgColor& color)
{
  mgString key;
  key.format("SolidPaint/%g,%g,%g,%g", color.m_r, color.m_g, color.m_b, color.m_a);
  
  mgSolidPaint* paint = (mgSolidPaint*) surface->findResource(key);
  if (paint != NULL)
    return paint;
    
  paint = new mgSolidPaint();
  paint->m_key = key;
  paint->m_brush = surface->createBrush(color);
  
  surface->saveResource(paint);
  return paint;
}
   
//--------------------------------------------------------------
// constructor
mgSolidPaint::mgSolidPaint()
{
  m_brush = NULL;
}

//--------------------------------------------------------------
// destructor
mgSolidPaint::~mgSolidPaint()
{
}

//--------------------------------------------------------------
// apply paint to rectangle
void mgSolidPaint::paint(
  mgContext* gc,
  int x,
  int y,
  int width,
  int height) const
{
  gc->setBrush(m_brush);
  gc->fillRect(x, y, width, height);
}
