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

#ifndef MGTOPCONTROL_H
#define MGTOPCONTROL_H

#include "mgControl.h"
#include "mgControlListener.h"
#include "mgTimeListener.h"

/*
  An mgTopControl instance is created on the rendering mgSurface and 
  sets the mgStyle for the entire tree of controls.  mgTopControl
  delivers events and handles redraws of modified controls.
*/
class mgTopControl : public mgControl, public mgControlListener
{
public:
  // constructor
  mgTopControl(
    mgSurface* surface,
    mgStyle* style);

  // destructor
  virtual ~mgTopControl();
  
  // return control to use as parent for menu panes, other popups
  virtual mgControl* getPage();
  
  // return UI style 
  virtual mgStyle* getStyle();
  
  // surface repainted
  virtual void surfacePaint(
    const mgRectangle& bounds);
    
  // surface has resized
  virtual void surfaceResized(
    int width,
    int height);

  // damage rectangle within control
  virtual void damage(
    int x,
    int y,
    int width,
    int height);
  
  // get graphics context to draw with
  virtual mgContext* newContext();

  // get rendering surface
  virtual mgSurface* getSurface();

  // set the key focus
  virtual void setKeyFocus(
    mgControl* focus);

  // get the key focus
  virtual mgControl* getKeyFocus();

  // get the mouse focus
  virtual mgControl* getMouseFocus();

  // process key down
  virtual void surfaceKeyDown(
    int key,
    int modifiers);
    
  // process key up 
  virtual void surfaceKeyUp(
    int key,
    int modifiers);
    
  // process key typed
  virtual void surfaceKeyChar(
    int key,
    int modifiers);
    
  // mouse entered surface
  virtual void surfaceMouseEnter(
    int x,
    int y);

  // mouse exited surface
  virtual void surfaceMouseExit();

  // mouse pressed
  virtual void surfaceMouseDown(
    int x, 
    int y,
    int modifiers,
    int button);

  // mouse released
  virtual void surfaceMouseUp(
    int x,
    int y,
    int modifiers,
    int button);

  // mouse double clicked
  virtual void surfaceMouseClick(
    int x,
    int y,
    int modifiers,
    int button,
    int clickCount);

  // mouse dragged
  virtual void surfaceMouseDrag(
    int x, 
    int y, 
    int modifiers);

  // mouse moved
  virtual void surfaceMouseMove(
    int x, 
    int y, 
    int modifiers);

  // add time listener
  virtual void addTimeListener(
    mgTimeListener* listener);

  // remove time listener
  virtual void removeTimeListener(
    mgTimeListener* listener);

  // update time listeners
  virtual void animate(
    double now = 0,
    double since = 0);

  // control resize
  virtual void controlResize(
    void* source);

  // control moved
  virtual void controlMove(
    void* source);

  // control shown
  virtual void controlShow(
    void* source);

  // control hidden
  virtual void controlHide(
    void* source);

  // control enabled
  virtual void controlEnable(
    void* source);

  // control disabled
  virtual void controlDisable(
    void* source);

  // control deleted
  virtual void controlDelete(
    void* source);

  // control child added
  virtual void controlAddChild(
    void* source);

  // control child removed
  virtual void controlRemoveChild(
    void* source);

protected:
  mgSurface* m_surface;
  mgStyle* m_style;

  mgControl* m_keyFocus;
  mgControl* m_mouseFocus;
  mgControl* m_mouseTarget;
  mgPoint m_mouseOrigin;

  mgPtrArray* m_timeListeners;
  double m_lastAnimate;

  // change mouse target
  virtual void changeMouseTarget(
    mgControl* target,
    int x,
    int y);

/*
  // inform topcontrol of cursor change
  virtual void cursorChanged(
    mgControl* source,
    const mgCursor* cursor);
*/
};

#endif

