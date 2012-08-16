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
#ifndef MGFIELDCONTROL_H
#define MGFIELDCONTROL_H

/*
  The parent of all text field controls.
*/
#include "mgControl.h"

class mgFieldControl : public mgControl
{
public:
  // constructor
  mgFieldControl(
    mgControl* parent,
    const char* cntlName)
  : mgControl(parent, cntlName)
  {}
    
  // destructor
  virtual ~mgFieldControl()
  {}
  
  // set number of chars to display
  virtual void setDisplayCount(
    int count) = 0;

  // set text in field
  virtual void setText(
    const char* text) = 0;

  // get value in field
  virtual void getText(
    mgString& text) = 0;

  // reset the field
  virtual void reset() = 0;

  // enable input history
  virtual void enableHistory(
    BOOL enable) = 0;

  // add a change listener
  virtual void addChangeListener(
    mgChangeListener* listener) = 0;

  // remove a change listener
  virtual void removeChangeListener(
    mgChangeListener* listener) = 0;

};

#endif
