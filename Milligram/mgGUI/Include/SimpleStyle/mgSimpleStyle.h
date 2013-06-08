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

#ifndef MGSIMPLESTYLE_H
#define MGSIMPLESTYLE_H

#include "../GUI/mgStyle.h"

/*
  The mgStyle sublass which creates all ugly controls.
*/
class mgSimpleStyle : public mgStyle
{
public:
  // constructor
  mgSimpleStyle(
    mgSurface* surface);

  // destructor
  virtual ~mgSimpleStyle();

  // create a label control
  virtual mgLabelControl* createLabel(
    mgControl* parent,
    const char* name, 
    const char* label);

  // create a field control
  virtual mgFieldControl* createField(
    mgControl* parent,
    const char* name);

  // create a button control
  virtual mgButtonControl* createButton(
    mgControl* parent,
    const char* name, 
    const char* label,
    const char* iconName);

  // create a checkbox control
  virtual mgCheckboxControl* createCheckbox(
    mgControl* parent,
    const char* name, 
    const char* label);

  // create a list control
  virtual mgListControl* createList(
    mgControl* parent,
    const char* name);

  // create a scrollpane control
  virtual mgScrollPaneControl* createScrollPane(
    mgControl* parent,
    const char* name);

  // create a console control
  virtual mgConsoleControl* createConsole(
    mgControl* parent,
    const char* name);

  // create a stack control
  virtual mgStackControl* createStack(
    mgControl* parent,
    const char* name);

  // create a split control
  virtual mgSplitControl* createSplit(
    mgControl* parent,
    const char* name);

  // create a tabbed control
  virtual mgTabbedControl* createTabbed(
    mgControl* parent,
    const char* name);

  // create a desktop control
  virtual mgDesktopControl* createDesktop(
    mgControl* parent,
    const char* name);

protected:
  mgSurface* m_surface;
};

#endif
