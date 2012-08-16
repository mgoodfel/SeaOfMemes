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
#ifndef MGUGLYFIELD_H
#define MGUGLYFIELD_H

#include "GUI/mgFieldControl.h"
#include "GUI/mgKeyListener.h"
#include "GUI/mgMouseListener.h"
#include "GUI/mgFocusListener.h"
#include "GUI/mgChangeListener.h"

class mgFrame;

class mgUglyField : public mgFieldControl, public mgKeyListener, public mgMouseListener, 
     public mgFocusListener
{
public:
  // constructor
  mgUglyField(
    mgControl* parent,
    const char* cntlName = NULL);
    
  // destructor
  virtual ~mgUglyField();
  
  // set number of chars to display
  virtual void setDisplayCount(
    int count);

  // set text in field
  virtual void setText(
    const char* text);

  // get value in field
  virtual void getText(
    mgString& text);

  // reset the field
  virtual void reset();
  
  // enable input history
  virtual void enableHistory(
    BOOL enable);

  // add a change listener
  virtual void addChangeListener(
    mgChangeListener* listener);

  // remove a change listener
  virtual void removeChangeListener(
    mgChangeListener* listener);

protected:
  mgString m_text;
  int m_displayCount;
  int m_cursorPosn;
  int m_scrollPosn;

  BOOL m_insertMode;
  BOOL m_changed;
  BOOL m_hasMouse;

  mgStringArray* m_history;
  int m_historyPosn;
  
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

  mgPtrArray* m_changeListeners;

  // get cursor X coordinate
  virtual int getCursorX(
    const mgString& strValue);

  // adjust scroll posn after change in cursor posn
  virtual void updateScrollPosn();

  // send event to change listeners
  virtual void sendChangeEvent();

  // key pressed
  virtual void keyDown(
    void* source,
    int key,
    int modifiers);

  // key released
  virtual void keyUp(
    void* source,
    int key,
    int modifiers);

  // character typed
  virtual void keyChar(
    void* source,
    int key,
    int modifiers);

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

  virtual void guiFocusGained(
    void* source);
    
  virtual void guiFocusLost(
    void* source);

  // return minimum size
  virtual void minimumSize(
    mgDimension& size);

  // return preferred size
  virtual void preferredSize(
    mgDimension& size);
    
  // get current frame and text color from state
  virtual void getFrame(
    const mgFrame*& frame,
    mgColor& textColor);

  // paint content of control
  virtual void paint(
    mgContext* gc);
};

#endif
