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

#ifndef MGPAINT_H
#define MGPAINT_H

class mgControl;

// superclass of all paint types
class mgPaint : public mgResource
{
public:
  // destructor
  virtual ~mgPaint()
  {}
    
  // apply paint to rectangle
  virtual void paint(
    mgContext* gc,
    int x,
    int y,
    int width,
    int height) const = 0;

  // apply paint to rectangle
  void paint(
    mgContext* gc,
    const mgRectangle& bounds) const
  {
    paint(gc, bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height); 
  }
};

#endif
