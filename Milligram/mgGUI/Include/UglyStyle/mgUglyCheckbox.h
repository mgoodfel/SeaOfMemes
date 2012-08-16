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
#ifndef MGUGLYCHECKBOX_H
#define MGUGLYCHECKBOX_H

#include "GUI/mgCheckboxControl.h"
#include "GUI/mgMouseListener.h"
#include "GUI/mgSelectListener.h"

class mgFrame;

class mgUglyCheckbox : public mgCheckboxControl, public mgMouseListener
{
public:
  // constructor
  mgUglyCheckbox(
    mgControl* parent,
    const char* cntlName = NULL);
    
  // destructor
  virtual ~mgUglyCheckbox();
  
  // set text
  virtual void setLabel(
    const char* label);
    
  // set state
  virtual void setState(
    BOOL state);
    
  // get state
  virtual BOOL getState();

  // add a select listener
  virtual void addSelectListener(
    mgSelectListener* listener);

  // remove a select listener
  virtual void removeSelectListener(
    mgSelectListener* listener);

protected:  
  mgString m_label;
  BOOL m_state;
  int m_mode;

  mgPtrArray* m_selectListeners;             // select listeners

  const mgFrame* m_upFrame;
  const mgFrame* m_hoverFrame;
  const mgFrame* m_downFrame;
  const mgFrame* m_disFrame;

  // text color
  mgColor m_upColor;
  mgColor m_hoverColor;
  mgColor m_downColor;
  mgColor m_disColor;

  // font
  const mgFont* m_font;

  // paint content of control
  virtual void paint(
    mgContext* gc);

  virtual void minimumSize(
    mgDimension& size);

  virtual void preferredSize(
    mgDimension& size);
    
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

  // send event to select listeners
  virtual void sendSelectEvent();
};

#endif
