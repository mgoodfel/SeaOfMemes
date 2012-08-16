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

#ifndef MGMENU_H
#define MGMENU_H

/*
  definition of a menu
*/
#include "GUI/mgControl.h"

class mgMenu
{
public:
  // constructor
  mgMenu();

  // destructor
  virtual ~mgMenu();
  
  // add menu item
  virtual void addMenuAction(
    const char* label,
    const mgIcon* icon,
    const char* name,
    int accelChar = 0,
    const char* keyEquiv = NULL);
    
  // add check menu item
  virtual void addMenuToggle(
    const char* label,
    const char* name,
    int accelChar = 0,
    const char* keyEquiv = NULL);

  // add submenu item
  virtual void addSubMenu(
    const char* label,
    mgMenu* subMenu, 
    int accelChar = 0);

  // add separator
  virtual void addSeparator();

  // enable/disable item
  void setItemEnabled(
    const char* name,
    BOOL enabled);

  // set toggle item state
  void setItemState(
    const char* name,
    BOOL state);
};

#endif
