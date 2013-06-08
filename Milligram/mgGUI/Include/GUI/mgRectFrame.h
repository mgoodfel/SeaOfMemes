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

#ifndef MGRECTFRAME_H
#define MGRECTFRAME_H

#include "mgFrame.h"

/*
  The various edge types of an mgRectFrame.
*/
enum mgRectEdgeType
{
  mgFlatEdge = 0,
  mgRaisedEdge = 1,
  mgLoweredEdge = 2,
};

/*
  An mgFrame that draws a rectangular border around the object.
*/
class mgRectFrame : public mgFrame
{
public:
  // constructor
  static const mgRectFrame* createFrame(
    mgSurface* surface,
    mgRectEdgeType edgeType,
    int edgeWidth,
    const mgColor& baseColor,
    const mgPaint* paint,
    const mgFrameMargin& margin);
  
  // destructor
  virtual ~mgRectFrame();
  
  // return size to frame requested area
  virtual void getOutsideSize(
    const mgDimension& interior,
    mgDimension& exterior) const;

  // adjust rectangle to exterior of frame
  virtual void getOutsideRect(
    mgRectangle& bounds) const;

  // adjust rectangle to interior of frame
  virtual void getInsideRect(
    mgRectangle& bounds) const;

  // paint the frame foreground
  virtual void paintForeground(
    mgContext* gc,
    int x,
    int y,
    int width,
    int height) const;

protected:
  mgRectEdgeType m_edgeType;
  int m_edgeWidth;
  const mgPen* m_highPen;
  const mgPen* m_lowPen;
  const mgPen* m_edgePen;
  const mgPen* m_blackPen;
  
  // constructor
  mgRectFrame();
};

#endif
