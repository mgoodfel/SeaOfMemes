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

#ifndef MGCHECKBOXCONTROL_H
#define MGCHECKBOXCONTROL_H

/*
  The parent of all checkbox controls.
*/
#include "mgControl.h"

class mgCheckboxControl : public mgControl
{
public:
  // constructor
  mgCheckboxControl(
    mgControl* parent,
    const char* cntlName)
  : mgControl(parent, cntlName)
  {}

  // destructor
  virtual ~mgCheckboxControl()
  {}
  
  // set button label
  virtual void setLabel(
    const char* label) = 0;

  // set state
  virtual void setState(
    BOOL state) = 0;
    
  // get state
  virtual BOOL getState() = 0;

  // add a select listener
  virtual void addSelectListener(
    mgSelectListener* listener) = 0;

  // remove a select listener
  virtual void removeSelectListener(
    mgSelectListener* listener) = 0;
};

#endif
