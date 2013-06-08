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

#include "GUI/mgPaint.h"
#include "GUI/mgRectFrame.h"

//--------------------------------------------------------------
// constructor
const mgRectFrame* mgRectFrame::createFrame(
  mgSurface* surface,
  mgRectEdgeType edgeType,
  int edgeWidth,
  const mgColor& baseColor,
  const mgPaint* paint,
  const mgFrameMargin& margin)
{
  mgString paintKey;
  if (paint != NULL)
    paintKey = paint->m_key;
    
  mgString key;
  key.format("RectFrame/%d,%d,%g,%g,%g,%g,%d,%d,%d,%d:%s", 
        edgeType, edgeWidth, 
        baseColor.m_r, baseColor.m_g, baseColor.m_b, baseColor.m_a, 
        margin.m_left, margin.m_top, margin.m_right, margin.m_bottom, 
        (const char*) paintKey);
  
  mgRectFrame* frame = (mgRectFrame*) surface->findResource(key);
  if (frame != NULL)
    return frame;
    
  frame = new mgRectFrame();
  frame->m_key = key;
  frame->m_edgeType = edgeType;
  frame->m_edgeWidth = edgeWidth;
  frame->m_paint = paint;
  frame->m_margin = margin;

  double r = min(1.0, baseColor.m_r * 1.5);
  double g = min(1.0, baseColor.m_g * 1.5);
  double b = min(1.0, baseColor.m_b * 1.5);
  frame->m_highPen = surface->createPen(1.0, r, g, b);

  r = baseColor.m_r * 0.7;
  g = baseColor.m_g * 0.7;
  b = baseColor.m_b * 0.7;
  frame->m_lowPen = surface->createPen(1.0, r, g, b);
  frame->m_edgePen = surface->createPen(edgeType == mgFlatEdge ? edgeWidth : 1.0, baseColor);
  frame->m_blackPen = surface->createPen(1.0, "black");
  
  surface->saveResource(frame);
  return frame;
}

//--------------------------------------------------------------
// constructor
mgRectFrame::mgRectFrame()
{
}

//--------------------------------------------------------------
// destructor
mgRectFrame::~mgRectFrame()
{
}

//--------------------------------------------------------------
// return size to frame requested area
void mgRectFrame::getOutsideSize(
  const mgDimension& interior,
  mgDimension& exterior) const
{
  exterior.m_width = interior.m_width + m_margin.m_left + m_margin.m_right;
  exterior.m_height = interior.m_height + m_margin.m_top + m_margin.m_bottom;

  switch (m_edgeType)
  {
    case mgFlatEdge:
      exterior.m_width += m_edgeWidth;
      exterior.m_height += m_edgeWidth;
      break;
      
    case mgRaisedEdge:
      exterior.m_width += 3;
      exterior.m_height += 3;
      break;

    case mgLoweredEdge:
      exterior.m_width += 3;
      exterior.m_height += 3;
      break;
  }
}

//--------------------------------------------------------------
// adjust rectangle to exterior of frame
void mgRectFrame::getOutsideRect(
  mgRectangle& bounds) const
{
  bounds.m_x -= m_margin.m_left;
  bounds.m_y -= m_margin.m_top;
  bounds.m_width += m_margin.m_left+m_margin.m_right;
  bounds.m_height += m_margin.m_top+m_margin.m_bottom;

  switch (m_edgeType)
  {
    case mgFlatEdge:
      bounds.m_x -= m_edgeWidth;
      bounds.m_y -= m_edgeWidth;
      bounds.m_width += 2*m_edgeWidth;
      bounds.m_height += 2*m_edgeWidth;
      break;
      
    case mgRaisedEdge:
      bounds.m_x -= 1;
      bounds.m_y -= 1;
      bounds.m_width += 3;
      bounds.m_height += 3;
      break;

    case mgLoweredEdge:
      bounds.m_x -= 2;
      bounds.m_y -= 2;
      bounds.m_width += 3;
      bounds.m_height += 3;
      break;
  }
}

//--------------------------------------------------------------
// adjust rectangle to interior of frame
void mgRectFrame::getInsideRect(
  mgRectangle& bounds) const
{
  bounds.m_x += m_margin.m_left;
  bounds.m_y += m_margin.m_top;
  bounds.m_width -= m_margin.m_left+m_margin.m_right;
  bounds.m_height -= m_margin.m_top+m_margin.m_bottom;

  switch (m_edgeType)
  {
    case mgFlatEdge:
      bounds.m_x += m_edgeWidth;
      bounds.m_y += m_edgeWidth;
      bounds.m_width -= 2*m_edgeWidth;
      bounds.m_height -= 2*m_edgeWidth;
      break;
      
    case mgRaisedEdge:
      bounds.m_x += 1;
      bounds.m_y += 1;
      bounds.m_width -= 3;
      bounds.m_height -= 3;
      break;

    case mgLoweredEdge:
      bounds.m_x += 2;
      bounds.m_y += 2;
      bounds.m_width -= 3;
      bounds.m_height -= 3;
      break;
  }
}

//--------------------------------------------------------------
// paint the frame background
void mgRectFrame::paintForeground(
  mgContext* gc,
  int x,
  int y,
  int width,
  int height) const
{
  switch (m_edgeType)
  {
    case mgFlatEdge:
      gc->setPen(m_edgePen);
      gc->drawRect(x, y, width, height);
      break;
      
    case mgRaisedEdge:
      // draw top and left edge
      gc->setPen(m_highPen);
      gc->drawLine(x, y+0.5, x+width-1, y+0.5);
      gc->drawLine(x+0.5, y+1, x+0.5, y+height-1);

      // draw bottom and right inside edge
      gc->setPen(m_lowPen);
      gc->drawLine(x+1, y+height-1.5, x+width-1, y+height-1.5);
      gc->drawLine(x+width-1.5, y+1, x+width-1.5, y+height-1);
      
      // draw bottom and right outside edge
      gc->setPen(m_blackPen);
      gc->drawLine(x, y+height-0.5, x+width, y+height-0.5);
      gc->drawLine(x+width-0.5, y, x+width-0.5, y+height);
      break;

    case mgLoweredEdge:
      // draw top and left outside edge
      gc->setPen(m_blackPen);
      gc->drawLine(x, y+0.5, x+width-1, y+0.5);
      gc->drawLine(x+0.5, y+1, x+0.5, y+height-1);
      
      // draw top and left inside edge
      gc->setPen(m_lowPen);
      gc->drawLine(x+1, y+1.5, x+width-1, y+1.5);
      gc->drawLine(x+1.5, y+2, x+1.5, y+height-1);
      
      // draw bottom and right edge
      gc->setPen(m_highPen);
      gc->drawLine(x+1, y+height-0.5, x+width, y+height-0.5);
      gc->drawLine(x+width-0.5, y+1, x+width-0.5, y+height);
      break;
  }
}

