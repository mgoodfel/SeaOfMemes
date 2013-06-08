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

#ifndef MGRADIOCONTROL_H
#define MGRADIOCONTROL_H

/*
  The superclass of all radio-type controls.
*/
#include "GUI/mgControl.h"

class mgRadioControl : public mgControl
{
public:
  // constructor
  mgRadioControl(
    mgControl* parent)
  : mgControl(parent)
  {}

  // destructor
  virtual ~mgRadioControl()
  {}
  
  // set button label
  virtual void setLabel(
    const char* text) = 0;

  // set radio group leader
  virtual void setGroup(
    const mgRadioControl* leader) = 0;

  // add a select listener
  virtual void addSelectListener(
    mgSelectListener* listener) = 0;

  // remove a select listener
  virtual void removeSelectListener(
    mgSelectListener* listener) = 0;
};

#endif
