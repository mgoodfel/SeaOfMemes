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

#ifndef MGBUTTONFACE_H
#define MGBUTTONFACE_H

#include "GenericStyle/mgGenericFace.h"

class mgGenericStyle;

class mgButtonFace : public mgGenericFace
{
public:
  // constructor
  mgButtonFace(
    mgGenericStyle* style,
    const char* key);

  // destructor
  virtual ~mgButtonFace();

  // get minimum size of control
  virtual void minimumSize(
    mgContext* gc,
    const mgGenericArgs& args,
    mgDimension& size) const;
    
  // get preferred size of control
  virtual void preferredSize(
    mgContext* gc,
    const mgGenericArgs& args,
    mgDimension& size) const;
    
  // draw in a rectangular area
  virtual void paint(
    mgContext* gc,
    const mgGenericArgs& args,
    const mgDimension& size) const;

protected:
  mgColor m_textColor;
  const mgFont* m_font;
  const mgFrame* m_frame;
};

#endif
