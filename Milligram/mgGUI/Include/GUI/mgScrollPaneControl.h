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

#ifndef MGSCROLLPANECONTROL_H
#define MGSCROLLPANECONTROL_H

/*
  The superclass of all scroll pane controls.  A scroll pane is 
  vertical and/or horizontal scrollbars controlling the view on
  a child control.
*/
#include "mgControl.h"

class mgScrollPaneControl : public mgControl
{
public:
  // constructor
  mgScrollPaneControl(
    mgControl* parent,
    const char* cntlName = NULL)
  : mgControl(parent, cntlName)
  {}

  // destructor
  virtual ~mgScrollPaneControl()
  {}
  
  // get scrolled pane parent
  virtual mgControl* getScrollParent() = 0;
    
  // set vertical scrollbar
  virtual void setVScrollbar(
    mgScrollbarControl* scrollbar) = 0;

  // set horizontal scrollbar
  virtual void setHScrollbar(
    mgScrollbarControl* scrollbar) = 0;
};

#endif
