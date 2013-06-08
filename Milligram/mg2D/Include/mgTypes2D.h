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

/*
  Basic types used by the 2D interface.
*/

#ifndef MGTYPES_H
#define MGTYPES_H

/*
  An integer x,y point.
*/
class mgPoint
{
public:
  int m_x;
  int m_y;
  
  // constructor
  mgPoint() 
  {
    m_x = 0;
    m_y = 0;
  }
  
  // constructor
  mgPoint(
    int x,
    int y)
  {
    m_x = x;
    m_y = y;
  }
};

/*
  A rectangle with integer x, y, width and height.
*/
class mgRectangle
{
public:
  int m_x;
  int m_y;
  int m_width;
  int m_height;
  
  // constructor
  mgRectangle() 
  {
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;
  }
  
  // constructor
  mgRectangle(
    int x,
    int y,
    int width,
    int height)
  {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
  }
  
  // copy constructor
  mgRectangle(
    const mgRectangle& other)
  {
    m_x = other.m_x;
    m_y = other.m_y;
    m_width = other.m_width;
    m_height = other.m_height;
  }

  // assignment
  mgRectangle& operator=(
    const mgRectangle& other)
  {
    m_x = other.m_x;
    m_y = other.m_y;
    m_width = other.m_width;
    m_height = other.m_height;
    return *this;
  }

  // return left
  int left() const
  {  return m_x; }
  
  // return top 
  int top() const
  { return m_y; }
  
  // return right 
  int right() const
  { return m_x + m_width; }
  
  // return bottom
  int bottom() const
  { return m_y + m_height; }
  
  // true if contains point
  BOOL containsPt(
    const mgPoint& pt) const
  {
    return pt.m_x > m_x && pt.m_x < m_x+m_width &&
           pt.m_y > m_y && pt.m_y < m_y+m_height;
  }
  
  // return true if empty
  BOOL isEmpty() const
  {
    return m_width <= 0.0 || m_height <= 0.0;
  }

  // set empty
  void empty()
  {
    m_x = m_y = 0;
    m_width = m_height = 0;
  }

  // is equal
  BOOL equals(
    mgRectangle &other)
  {
    return m_x == other.m_x && m_y == other.m_y && 
           m_width == other.m_width && m_height == other.m_height;
  }
      
  // intersect a rect with this rect
  BOOL intersectRect(
    const mgRectangle& other)
  {
    int lx = max(m_x, other.m_x);
    int ly = max(m_y, other.m_y);
    int hx = min(right(), other.right());
    int hy = min(bottom(), other.bottom());
    m_x = lx;
    m_y = ly;
    m_width = max(0, hx - lx);
    m_height = max(0, hy - ly);
    
    return !isEmpty();
  }
  
  // union a rect with this rect
  void unionRect(
    const mgRectangle& other)
  {
    int lx = min(m_x, other.m_x);
    int ly = min(m_y, other.m_y);
    int hx = max(right(), other.right());
    int hy = max(bottom(), other.bottom());
    m_x = lx;
    m_y = ly;
    m_width = hx - lx;
    m_height = hy - ly;
  }
};

/*
  An integer width and height.
*/
class mgDimension
{
public:
  int m_width;
  int m_height;
  
  // constructor
  mgDimension() 
  {
    m_width = 0;
    m_height = 0;
  }
  
  // constructor
  mgDimension(
    int width,
    int height)
  {
    m_width = width;
    m_height = height;
  }
};

#include "mgResource.h"
#include "mgColor.h"
#include "mgPen.h"
#include "mgBrush.h"
#include "mgFont.h"
#include "mgImage.h"
#include "mgIcon.h"

#endif
