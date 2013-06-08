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

#ifndef MGLAYOUTMANAGER_H
#define MGLAYOUTMANAGER_H

#include "mgControl.h"

/*
  Abstract interface to layout managers (ex: TableLayout).  The layout manager
  is called by the framework when the parent control is resized.
*/
class mgLayoutManager
{
public:
  // constructor
  mgLayoutManager(
    mgControl* parent)
  {
    m_parent = parent;
    m_parent->setLayout(this);
  }
  
  // destructor
  virtual ~mgLayoutManager()
  {
  }
  
  // remove child from layout
  virtual void removeChild(
    mgControl* child) = 0;
  
  // return min size of control
  virtual void minimumSize(
    mgDimension& size) = 0;

  // return preferred size of control
  virtual void preferredSize(
    mgDimension& size) = 0;

  // compute size at width.  return false if not implemented
  virtual BOOL preferredSizeAtWidth(
    int width,
    mgDimension& size) = 0;

  // adjust layout after resize
  virtual void controlResized() = 0;

  // paint background graphics
  virtual void paintBackground(
    mgContext* gc) = 0;

  // paint foreground graphics
  virtual void paintForeground(
    mgContext* gc) = 0;
    
protected:
  mgControl* m_parent;    
};

#endif


