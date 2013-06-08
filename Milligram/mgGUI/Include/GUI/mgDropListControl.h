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

#ifndef MGDROPLISTCONTROL_H
#define MGDROPLISTCONTROL_H

/*
  The superclass of all drop list controls.
*/
#include "GUI/mgControl.h"

class mgDropListControl : public mgControl
{
public:
  // constructor
  mgDropListControl(
    mgControl* parent,
    const char* cntlName = NULL)
  : mgControl(parent, cntlName)
  {}

  // destructor
  virtual ~mgDropListControl()
  {}
  
  // add a list choice
  virtual void addEntry(
    const char* label,
    const char* name) = 0;
     
  // true if supports multiselect
  virtual BOOL canMultiSelect() = 0;

  // set multiple select mode
  virtual void setMultiSelect(
    BOOL multiSelect) = 0;

  // set item selected
  virtual void setItemState(
    const char* name,
    BOOL selected) = 0;

  // return true if item selected
  virtual BOOL getItemState(
    const char* name) = 0;

  // get count of selected items
  virtual int getSelectionCount() = 0;
    
  // get nth selected item
  virtual void getSelectedItem(
    int posn,
    mgString &name) = 0;
    
  // add a select listener
  virtual void addSelectListener(
    mgSelectListener* listener);

  // remove a select listener
  virtual void removeSelectListener(
    mgSelectListener* listener);
};

#endif
