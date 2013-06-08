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
#ifndef MGSIMPLETABS_H
#define MGSIMPLETABS_H

#include "mgControl.h"

/*
  The parent of tabbed page controls.  Selecting a tab shows one of the child controls.
*/
class mgTabbedControl : public mgControl
{
public:
  // constructor
  mgTabbedControl(
    mgControl* parent,
    const char* cntlName = NULL)
  : mgControl(parent, cntlName)
  {}
    
  // destructor
  virtual ~mgTabbedControl()
  {}
  
  // add a pane to the stack
  virtual mgControl* addPane(
    const char* label,
    const char* name) = 0;
    
  // remove a pane from the stack
  virtual void removePane(
    const char* name) = 0;
    
  // select pane to show
  virtual void selectPane(
    const char* name) = 0;
    
  // get selected pane
  virtual void getSelected(
    mgString& name) = 0;
};

#endif
