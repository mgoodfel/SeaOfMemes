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

#ifndef MGSIMPLEBUTTON_H
#define MGSIMPLEBUTTON_H

#include "GUI/mgButtonControl.h"
#include "GUI/mgMouseListener.h"
#include "GUI/mgFocusListener.h"
#include "GUI/mgActionListener.h"

/*
  A push-button control with text label.
*/
class mgSimpleButton : public mgButtonControl, public mgMouseListener, public mgFocusListener
{
public:
  // constructor
  mgSimpleButton(
    mgControl* parent,
    const char* cntlName = NULL);

  // destructor
  virtual ~mgSimpleButton();
  
  // set button label
  virtual void setLabel(
    const char* text);

  // set button icon
  virtual void setIcon(
    const char* fileName);

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

protected:    
  int m_mode;
  mgString m_label;
  mgString m_iconName;

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

  mgPtrArray* m_actionListeners;             // action listeners
  
  // paint content of control
  virtual void paint(
    mgContext* gc);

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

  // focus gained
  virtual void guiFocusGained(
    void* source);
    
  // focus lost
  virtual void guiFocusLost(
    void* source);
};

#endif
