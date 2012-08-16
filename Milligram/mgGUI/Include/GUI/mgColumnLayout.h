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
#ifndef MGCOLUMNLAYOUT_H
#define MGCOLUMNLAYOUT_H

#include "mgLayoutManager.h"

class mgFrame;

class mgColumnLayout : public mgLayoutManager
{
public:
  // constructor
  mgColumnLayout(
    mgControl* parent);
    
  // destructor
  virtual ~mgColumnLayout();

  // set frame for children
  virtual void setFrame(
    const mgFrame* frame);
      
  // remove child from layout
  virtual void removeChild(
    mgControl* child);
  
  // return minimum size
  virtual void minimumSize(
    mgDimension& size);

  // return preferred size
  virtual void preferredSize(
    mgDimension& size);

  // compute size at width.  return false if not implemented
  virtual BOOL preferredSizeAtWidth(
    int width,
    mgDimension& size);

  virtual void controlResized();

  virtual void paintBackground(
    mgContext* gc);

  virtual void paintForeground(
    mgContext* gc);

protected:
  const mgFrame* m_frame;
};

#endif
