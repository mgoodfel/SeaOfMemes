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
#ifndef MGSIMPLEMENUBAR_H
#define MGSIMPLEMENUBAR_H

#include "GUI/mgMenuBarControl.h"
#include "GUI/mgMouseListener.h"
#include "GUI/mgSelectListener.h"

/*
  A menu bar control.
*/
class mgSimpleMenuBar : public mgMenuBarControl, public mgMouseListener
{
public:
  // constructor
  mgSimpleMenuBar(
    mgControl* parent);

  // destructor
  virtual ~mgSimpleMenuBar();
  
  // set menu
  virtual void setMenu(
    const mgMenu* menu);

  // return minimum size
  virtual void minimumSize(
    mgDimension& size);

  // return preferred size
  virtual void preferredSize(
    mgDimension& size);
    
  // add a action listener
  virtual void addActionListener(
    mgActionListener* listener);

  // remove a action listener
  virtual void removeActionListener(
    mgActionListener* listener);

  // add a select listener
  virtual void addSelectListener(
    mgSelectListener* listener);

  // remove a select listener
  virtual void removeSelectListener(
    mgSelectListener* listener);

protected:
  mgPtrArray* m_selectListeners;             // select listeners
  mgPtrArray* m_actionListeners;             // action listeners

  // send event to select listeners
  virtual void sendSelectEvent();

  // send event to action listeners
  virtual void sendActionEvent(
    const char* value);

  // mouse entered
  virtual void mouseEnter(
    void* source,
    int x,
    int y);

  // mouse exited
  virtual void mouseExit(
    void* source);

  // mouse pressed
  virtual void mouseDown(
    void* source,
    int x,
    int y,
    int modifiers,
    int button);

  // mouse released
  virtual void mouseUp(
    void* source,
    int x,
    int y,
    int modifiers,
    int button);

  // mouse clicked
  virtual void mouseClick(
    void* source,
    int x,
    int y,
    int modifiers,
    int button,
    int clickCount);

  // mouse dragged
  virtual void mouseDrag(
    void* source,
    int x,
    int y,
    int modifiers);

  // mouse moved
  virtual void mouseMove(
    void* source,
    int x,
    int y,
    int modifiers);

};

#endif
