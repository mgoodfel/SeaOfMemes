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

#ifndef MGSIMPLEWINDOW_H
#define MGSIMPLEWINDOW_H

#include "GUI/mgControl.h"
#include "GUI/mgActionListener.h"
#include "GUI/mgMouseListener.h"

class mgSimpleDesktop;
class mgSimpleButton;

/*
  A window frame used by mgSimpleDesktop
*/
class mgSimpleWindow : public mgControl, public mgActionListener, public mgMouseListener
{
public:
  mgControl* m_contentPane;        // parent of any window contents

  // constructor
  mgSimpleWindow(
    mgSimpleDesktop* desktop,
    mgControl* parent,
    const char* cntlName = NULL);

  // destructor
  virtual ~mgSimpleWindow();

  // set window title
  virtual void setTitle(
    const char* title);

  // set window flags
  virtual void setFlags(
    DWORD flags);

  // set window location relative to origin
  virtual void setWindowLocation(
    int origin,
    int offsetX,
    int offsetY);

  // set content location relative to origin
  virtual void setContentLocation(
    int origin,
    int offsetX,
    int offsetY);

  // initialize content size to preferred size
  virtual void initContentSize();

  // set requested content size
  virtual void setContentSize(
    int width, 
    int height);

  // get requested content size
  virtual void getContentSize(
    mgDimension &contentSize) const;

  // get requested bounds of window within desktop
  virtual void requestedBounds(
    mgDimension& desktopSize,
    mgRectangle& bounds);

  // paint content of control
  virtual void paint(
    mgContext* gc);

  virtual void minimumSize(
    mgDimension& size);

  virtual void preferredSize(
    mgDimension& size);

protected:
  mgSimpleDesktop* m_desktop;
  DWORD m_flags;

  int m_locationOrigin;
  mgRectangle m_locationBounds;
  BOOL m_locationContent;             // true if location is of content

  const mgFrame* m_onFrame;
  mgColor m_onColor;
  const mgFrame* m_offFrame;
  mgColor m_offColor;

  const mgFont* m_font;
  mgString m_title;

  mgSimpleButton* m_closeBtn;
  mgSimpleButton* m_minBtn;
  mgSimpleButton* m_maxBtn;

  int m_barHeight;                    // title area height
  int m_iconsWidth;                   // control button area
  int m_iconsHeight;                  // control button area

  int m_pressMode;                    // mouse press mode
  int m_dragX;                        // start position of drag
  int m_dragY;  

  // update layout of children
  virtual void updateLayout();


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

  // action performed
  virtual void guiAction(
    void* source,
    const char* name);

  friend class mgSimpleDesktop;
};

#endif