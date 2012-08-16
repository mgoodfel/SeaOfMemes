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

#ifndef MGGENERICLABEL_H
#define MGGENERICLABEL_H

#include "GUI/mgLabelControl.h"
#include "GenericStyle/mgGenericArgs.h"

class mgGenericFace;

class mgGenericLabel : public mgLabelControl
{
public:
  // constructor
  mgGenericLabel(
    mgControl* parent);

  // destructor
  virtual ~mgGenericLabel();

  // set the label
  virtual void setLabel(
    const char* text);

  // set the preferred size from a string
  virtual void setMaxLabel(
    const char* text);

  // paint content of control
  virtual void paint(
    mgContext* gc);

  virtual void minimumSize(
    mgDimension& size);

  virtual void preferredSize(
    mgDimension& size);

protected:
  mgString m_label;
  mgString* m_maxLabel;

  mgGenericArgs m_args;
  mgGenericFace* m_enabledFace;
  mgGenericFace* m_disabledFace;
};

#endif