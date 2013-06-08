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
#ifndef MGSIMPLEDESKTOP_H
#define MGSIMPLEDESKTOP_H

#include "GUI/mgDesktopControl.h"
#include "GUI/mgMouseListener.h"
#include "GUI/mgTimeListener.h"

class mgSimpleWindow;

/*
  A desktop-style window manager
*/
class mgSimpleDesktop: public mgDesktopControl, 
  public mgMouseListener, public mgTimeListener
{
public:
  // constructor
  mgSimpleDesktop(
    mgControl* parent,
    const char* cntlName = NULL);
    
  // destructor
  virtual ~mgSimpleDesktop();
  
  // add a window, return pane control
  virtual mgControl* addWindow(
    const char* windowName = NULL);

  // remove and delete a window
  virtual void removeWindow(
    mgControl* contentPane);

  // show the window
  virtual void showWindow(
    mgControl* contentPane);

  // hide the window without deleting it
  virtual void hideWindow(
    mgControl* contentPane);

  // surface window
  virtual void surfaceWindow(
    mgControl* contentPane);

  // layout the window to preferred size
  virtual void layoutWindow(
    mgControl* contentPane);

  // set window flags
  virtual void enableWindowFeatures(
    mgControl* contentPane,
    DWORD flags);

  // set window title
  virtual void setWindowTitle(
    mgControl* contentPane,
    const char* title);

  // set window location 
  virtual void setWindowLocation(
    mgControl* contentPane,
    DWORD origin,
    int offsetX,
    int offsetY);

  // set content location 
  virtual void setContentLocation(
    mgControl* contentPane,
    DWORD origin,
    int offsetX,
    int offsetY);

  // set content pane size
  virtual void setContentSize(
    mgControl* contentPane,
    int width,
    int height);
              
  // add a desktop listener
  virtual void addDesktopListener(
    mgDesktopListener* listener);

  // remove a desktop listener
  virtual void removeDesktopListener(
    mgDesktopListener* listener);

  // paint content of control
  virtual void paint(
    mgContext* gc);

  // return minimum size of control
  virtual void minimumSize(
    mgDimension& size);

  // return preferred size of control
  virtual void preferredSize(
    mgDimension& size);
    
protected:
  mgPtrArray m_desktopListeners;
  mgPtrArray m_windows;
  mgPtrArray m_deleted;

  // find the window by its content pane
  virtual mgSimpleWindow* findWindow(
    mgControl* contentPane);

  // update layout of children
  virtual void updateLayout();

  // close-button hit on window
  virtual void windowClose(
    mgControl* contentPane);

  // maximize-button hit on window
  virtual void windowMinimize(
    mgControl* contentPane);

  // minimize-button hit on window
  virtual void windowMaximize(
    mgControl* contentPane);

  // window moved
  virtual void windowMove(
    mgControl* contentPane);

  // window moved
  virtual void windowResize(
    mgControl* contentPane);

  // return true if active window
  virtual BOOL windowIsActive(
    mgControl* contentPane);

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

  // time has passed
  virtual void guiTimerTick(
    double now,
    double since);

  friend class mgSimpleWindow;
};

#endif
