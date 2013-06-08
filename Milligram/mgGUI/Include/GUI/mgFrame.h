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

#ifndef MGFRAME_H
#define MGFRAME_H

class mgControl;
class mgPaint;

/*
  The whitespace around the outside of a frame (in pixels.)
*/
class mgFrameMargin
{
public:
  int m_left;
  int m_top;
  int m_right;
  int m_bottom;
  
  // constructor
  mgFrameMargin()
  {
    m_left = m_top = 0;
    m_right = m_bottom = 0;
  }

  // constructor
  mgFrameMargin(
    int width)
  {
    m_left = m_top = width;
    m_right = m_bottom = width;
  }
  
  // constructor
  mgFrameMargin(
    int left,
    int top,
    int right,
    int bottom)
  {
    m_left = left;
    m_top = top;
    m_right = right;
    m_bottom = bottom;
  }
};

/*
  A frame around a rectangular area.  The interior can be painted with an mgPaint.
  This is a surface-managed resource and should not be deleted (see mg2D/mgSurface).
*/
class mgFrame : public mgResource
{
public:
  mgFrameMargin m_margin;
  const mgPaint* m_paint;

  // constructor
  static const mgFrame* createFrame(
    mgSurface* surface);

  // constructor
  static const mgFrame* createFrame(
    mgSurface* surface,
    const mgPaint* paint);

  // constructor
  static const mgFrame* createFrame(
    mgSurface* surface,
    const mgPaint* paint,
    const mgFrameMargin& margin);

  // destructor
  virtual ~mgFrame();
  
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

  // paint the frame background
  virtual void paintBackground(
    mgContext* gc,
    int x,
    int y,
    int width,
    int height) const;

  void paintBackground(
    mgContext* gc,
    const mgRectangle& bounds) const
  {
    paintBackground(gc, bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height);
  }

  // paint the frame foreground
  virtual void paintForeground(
    mgContext* gc,
    int x,
    int y,
    int width,
    int height) const;
    
  void paintForeground(
    mgContext* gc,
    const mgRectangle& bounds) const
  {
    paintForeground(gc, bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height);
  }

protected:
  // constructor
  mgFrame();
};

#endif
