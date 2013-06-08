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

#ifndef MGCONSOLECONTROL_H
#define MGCONSOLECONTROL_H

/*
  The superclass of all console controls.  A console is a scrolling text
  output window, such as a command line history.
*/
#include "mgControl.h"

class mgConsoleControl : public mgControl
{
public:
  // constructor
  mgConsoleControl(
    mgControl* parent,
    const char* cntlName)
  : mgControl(parent, cntlName)
  {}

  // destructor
  virtual ~mgConsoleControl()
  {}
  
  // add a line
  virtual void addLine(
    const mgColor& color,
    const mgFont* font,
    const char* text) = 0;

  // set preferred height
  virtual void setDisplayLines(
    int lines) = 0;

  // set history
  virtual void setHistoryLines(
    int lines) = 0;
};

#endif
