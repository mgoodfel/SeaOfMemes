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

#include "GUI/mgPaint.h"
#include "GUI/mgFrame.h"

//--------------------------------------------------------------
// constructor
const mgFrame* mgFrame::createFrame(
  mgSurface* surface)
{
  return createFrame(surface, NULL, mgFrameMargin(0));
}

//--------------------------------------------------------------
// constructor
const mgFrame* mgFrame::createFrame(
  mgSurface* surface,
  const mgPaint* paint)
{
  return createFrame(surface, paint, mgFrameMargin(0));
}

//--------------------------------------------------------------
// create frame or find in cache
const mgFrame* mgFrame::createFrame(
  mgSurface* surface,
  const mgPaint* paint,
  const mgFrameMargin& margin)
{
  mgString paintKey;
  if (paint != NULL)
    paintKey = paint->m_key;
    
  mgString key;
  key.format("SimpleFrame/%d,%d,%d,%d:%s", 
    margin.m_left, margin.m_top, margin.m_right, margin.m_bottom, 
    (const char*) paintKey);
  
  mgFrame* frame = (mgFrame*) surface->findResource(key);
  if (frame != NULL)
    return frame;
    
  frame = new mgFrame();
  frame->m_key = key;
  frame->m_paint = paint;
  frame->m_margin = margin;
  
  surface->saveResource(frame);
  return frame;
}

//--------------------------------------------------------------
// constructor
mgFrame::mgFrame()
{
}

//--------------------------------------------------------------
// destructor
mgFrame::~mgFrame()
{
}

//--------------------------------------------------------------
// return size to frame requested area
void mgFrame::getOutsideSize(
  const mgDimension& interior,
  mgDimension& exterior) const
{
  exterior.m_width = interior.m_width + m_margin.m_left + m_margin.m_right;
  exterior.m_height = interior.m_height + m_margin.m_top + m_margin.m_bottom;
}

//--------------------------------------------------------------
// adjust rectangle to interior of frame
void mgFrame::getInsideRect(
  mgRectangle& bounds) const
{
  bounds.m_x += m_margin.m_left;
  bounds.m_y += m_margin.m_top;
  bounds.m_width -= m_margin.m_left+m_margin.m_right;
  bounds.m_height -= m_margin.m_top+m_margin.m_bottom;
}

//--------------------------------------------------------------
// paint the frame background
void mgFrame::paintBackground(
  mgContext* gc,
  int x,
  int y,
  int width,
  int height) const
{
  if (m_paint != NULL)
    m_paint->paint(gc, x, y, width, height);
}

//--------------------------------------------------------------
// paint the frame foreground
void mgFrame::paintForeground(
  mgContext* gc,
  int x,
  int y,
  int width,
  int height) const
{
  // no edge on default frame
}
