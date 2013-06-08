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

#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "GUI/mgStyle.h"
#include "GUI/mgTopControl.h"

// =-= add mouse focus for handling case when button pressed, then second
// button pressed.  Second press should go to focus of first, not the 
// control under the cursor?

//--------------------------------------------------------------
// constructor
mgTopControl::mgTopControl(
  mgSurface* surface,
  mgStyle* style)
: mgControl(NULL, NULL)
{
  m_surface = surface;
  m_style = style;
  
  m_keyFocus = NULL;
  m_mouseTarget = NULL;
  m_mouseFocus = NULL;
  m_timeListeners = NULL;
  m_lastAnimate = mgOSGetTime();
}

//--------------------------------------------------------------
// destructor
mgTopControl::~mgTopControl() 
{
  // delete the time listeners
  delete m_timeListeners;
  m_timeListeners = NULL;
}

//--------------------------------------------------------------
// return control to use as parent for menu panes, other popups
mgControl* mgTopControl::getPage()
{
  return this;
}
  
//--------------------------------------------------------------
// return UI style 
mgStyle* mgTopControl::getStyle()
{
  return m_style;
}
  
//--------------------------------------------------------------
// surface repainted
void mgTopControl::surfacePaint(
  const mgRectangle& bounds)
{
  mgContext* gc = m_surface->newContext();

  // clear the area
  gc->resetState();
  gc->setClip(bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height);
  gc->setBrush(m_surface->createBrush(mgColor(0, 0, 0, 0)));
  gc->fillRect(bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height);

  gc->setClip(bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height);

  // repaint ourselves and all our children
  paintControl(gc, bounds.m_x, bounds.m_y, bounds.right(), bounds.bottom());
  delete gc;
}

//--------------------------------------------------------------
// surface has resized
void mgTopControl::surfaceResized(
  int surfaceWidth,
  int surfaceHeight) 
{
  setSize(surfaceWidth, surfaceHeight);

  if (m_children == NULL)
    return;

  // if layout present, children were positioned in setSize
  if (m_layout != NULL)
    return;
    
  // if no layout, stack the children in a column.  
  // Normally, there's a single child.
  int height = surfaceHeight/m_children->length();
  int y = 0;
  for (int i = 0; i < m_children->length(); i++)
  {
    mgControl* child = (mgControl*) m_children->getAt(i);
    child->setSize(surfaceWidth, height);
    child->setLocation(0, y);
    y += height;
  }
}

//--------------------------------------------------------------
// damage rectangle within control
void mgTopControl::damage(
  int x,
  int y,
  int width,
  int height) 
{
  m_surface->damage(x, y, width, height);
}

//--------------------------------------------------------------
// get graphics context to draw with
mgContext* mgTopControl::newContext() 
{
  return m_surface->newContext();
}

//--------------------------------------------------------------
// get rendering surface
mgSurface* mgTopControl::getSurface()
{
  return m_surface;
}

//--------------------------------------------------------------
// process key press
void mgTopControl::surfaceKeyDown(
  int key,
  int modifiers)
{
  if (m_keyFocus != NULL && m_keyFocus->getEnabled())
    m_keyFocus->dispatchKeyDown(this, key, modifiers);
}

//--------------------------------------------------------------
// process key release
void mgTopControl::surfaceKeyUp(
  int key,
  int modifiers)
{
  if (m_keyFocus != NULL && m_keyFocus->getEnabled())
    m_keyFocus->dispatchKeyUp(this, key, modifiers);
}

//--------------------------------------------------------------
// process key typed
void mgTopControl::surfaceKeyChar(
  int key,
  int modifiers)
{
  if (m_keyFocus != NULL && m_keyFocus->getEnabled())
    m_keyFocus->dispatchKeyChar(this, key, modifiers);
}

//--------------------------------------------------------------
// set the key focus
void mgTopControl::setKeyFocus(
  mgControl* focus)
{
  if (m_keyFocus == focus)
    return;  // nothing to do

  if (m_keyFocus != NULL)
  {
    // reset focus in case queried or changed by current focus
    mgControl* oldFocus = m_keyFocus;
    m_keyFocus = NULL;
    oldFocus->dispatchFocusLost(this);
    oldFocus->removeControlListener(this);
  }

  m_keyFocus = focus;

  if (m_keyFocus != NULL)
  {
    m_keyFocus->addControlListener(this);
    m_keyFocus->dispatchFocusGained(this);
  }
}

//--------------------------------------------------------------
// get the key focus
mgControl* mgTopControl::getKeyFocus()
{
  return m_keyFocus;
}

//--------------------------------------------------------------
// get the mouse focus
mgControl* mgTopControl::getMouseFocus()
{
  return m_mouseFocus;
}

#ifdef WORKED
//--------------------------------------------------------------
// release the mouse focus
void mgTopControl::releaseMouseFocus()
{
  if (m_mouseTarget == this)
    m_mouseTarget = NULL;
}
#endif

//--------------------------------------------------------------
// change mouse target
void mgTopControl::changeMouseTarget(
  mgControl* target,
  int x,
  int y)
{
  if (m_mouseTarget == target)
    return;
    
  if (m_mouseTarget != NULL)
  {
    m_mouseTarget->dispatchMouseExit(this);
    m_mouseTarget->removeControlListener(this);
  }
  
  m_mouseTarget = target;
  
  if (m_mouseTarget != NULL)
  {
    m_mouseTarget->addControlListener(this);

    m_mouseOrigin.m_x = 0;
    m_mouseOrigin.m_y = 0;
    m_mouseTarget->getLocationInAncestor(this, m_mouseOrigin);
    m_mouseTarget->dispatchMouseEnter(this, x - m_mouseOrigin.m_x, y - m_mouseOrigin.m_y);
    
//    const mgCursor* cursor = m_mouseTarget->getCursor();
//    m_surface->setCursorPattern(cursor);
  }
}

//--------------------------------------------------------------
// mouse entered surface
void mgTopControl::surfaceMouseEnter(
  int x,
  int y)
{
  mgControl* target = findControlAtPoint(x, y);
  changeMouseTarget(target, x, y);
}

//--------------------------------------------------------------
// mouse exited surface
void mgTopControl::surfaceMouseExit()
{
  changeMouseTarget(NULL, 0, 0);
}

//--------------------------------------------------------------
// mouse pressed
void mgTopControl::surfaceMouseDown(
  int x, 
  int y,
  int modifiers,
  int button)
{
  // if no children added yet, ignore
  if (m_children == NULL)
    return;

  // =-= second mouse button down should not change focus
  
  // find target, which will at least be this control (not NULL)
  mgControl* target = findControlAtPoint(x, y);
  changeMouseTarget(target, x, y);
  
  if (m_mouseTarget != NULL && m_mouseTarget->getEnabled())
  {
    // control is mouse focus during a drag
    m_mouseFocus = m_mouseTarget;
    m_mouseTarget->dispatchMouseDown(m_mouseTarget, 
      x - m_mouseOrigin.m_x, y - m_mouseOrigin.m_y,
      modifiers, button);
  }
}

//--------------------------------------------------------------
// mouse released
void mgTopControl::surfaceMouseUp(
  int x,
  int y,
  int modifiers,
  int button)
{
  // if no children added yet, ignore
  if (m_children == NULL)
    return;

  // if no mouse focus, control was deleted or something.  find new target
  if (m_mouseTarget == NULL)
  {
    // find target, which will at least be this control (not NULL)
    mgControl* target = findControlAtPoint(x, y);

    // make target the mouse focus
    changeMouseTarget(target, x, y);
  }

  // send released to mouse focus
  if (m_mouseTarget != NULL && m_mouseTarget->getEnabled())
  {
    m_mouseTarget->dispatchMouseUp(m_mouseTarget, 
      x - m_mouseOrigin.m_x, y - m_mouseOrigin.m_y,
      modifiers, button);
    // there is no mouse focus
    m_mouseFocus = NULL;
  }
}

//--------------------------------------------------------------
// mouse double clicked
void mgTopControl::surfaceMouseClick(
  int x,
  int y,
  int modifiers,
  int button,
  int clickCount)
{
  // if no children added yet, ignore
  if (m_children == NULL)
    return;

  // if no mouse focus, control was deleted or something.  find new target
  if (m_mouseTarget == NULL)
  {
    // find target, which will at least be this control (not NULL)
    mgControl* target = findControlAtPoint(x, y);

    // make target the mouse focus
    changeMouseTarget(target, x, y);
  }

  // pass clicked to mouse focus
  if (m_mouseTarget != NULL && m_mouseTarget->getEnabled())
  {
    m_mouseTarget->dispatchMouseClick(m_mouseTarget, 
      x - m_mouseOrigin.m_x, y - m_mouseOrigin.m_y,
      modifiers, button, clickCount);
  }
}

//--------------------------------------------------------------
// mouse dragged
void mgTopControl::surfaceMouseDrag(
  int x, 
  int y, 
  int modifiers)
{
  // if no children added yet, ignore
  if (m_children == NULL)
    return;

  // if no mouse focus, control was deleted or something.  find new target
  if (m_mouseTarget == NULL)
  {
    // find target, which will at least be this control (not NULL)
    mgControl* target = findControlAtPoint(x, y);

    // make target the mouse focus
    changeMouseTarget(target, x, y);
  }

  if (m_mouseTarget != NULL && m_mouseTarget->getEnabled())
  {
    m_mouseTarget->dispatchMouseDrag(m_mouseTarget,
      x - m_mouseOrigin.m_x, y - m_mouseOrigin.m_y,
      modifiers);
  }
}

//--------------------------------------------------------------
// mouse moved
void mgTopControl::surfaceMouseMove(
  int x, 
  int y, 
  int modifiers)
{
  // if no children added yet, ignore
  if (m_children == NULL)
    return;

  mgControl* target = findControlAtPoint(x, y);
  changeMouseTarget(target, x, y);
  
  // send moved event
  if (m_mouseTarget != NULL && m_mouseTarget->getEnabled())
  {
    m_mouseTarget->dispatchMouseMove(m_mouseTarget,
      x - m_mouseOrigin.m_x, y - m_mouseOrigin.m_y,
      modifiers);
  }
}

//--------------------------------------------------------------
// add time listener
void mgTopControl::addTimeListener(
  mgTimeListener* listener)
{
  if (m_timeListeners == NULL)
    m_timeListeners = new mgPtrArray();
  m_timeListeners->add(listener);
}

//--------------------------------------------------------------
// remove time listener
void mgTopControl::removeTimeListener(
  mgTimeListener* listener)
{
  if (m_timeListeners == NULL)
    return;
  m_timeListeners->remove(listener);
}

//--------------------------------------------------------------
// update time listeners
void mgTopControl::animate(
  double now,
  double since)
{
  if (m_timeListeners == NULL)
    return;
  
  // get animation times if not supplied
  if (now == 0)
    now = mgOSGetTime();
  if (since == 0)
    since = now - m_lastAnimate;
  m_lastAnimate = now;
      
  for (int i = 0; i < m_timeListeners->length(); i++)
  {
    mgTimeListener* listener = (mgTimeListener*) m_timeListeners->getAt(i);
    listener->guiTimerTick(now, since);
  }
}

//--------------------------------------------------------------
// listener for control resize
void mgTopControl::controlResize(
  void* source)
{
}

//--------------------------------------------------------------
// listener for control moved
void mgTopControl::controlMove(
  void* source)
{
}

//--------------------------------------------------------------
// listener for control shown
void mgTopControl::controlShow(
  void* source)
{
}

//--------------------------------------------------------------
// listener for control hidden
void mgTopControl::controlHide(
  void* source)
{
}

//--------------------------------------------------------------
// listener for control enabled
void mgTopControl::controlEnable(
  void* source)
{
}

//--------------------------------------------------------------
// listener for control disabled
void mgTopControl::controlDisable(
  void* source)
{
}

//--------------------------------------------------------------
// listener for control deleted
void mgTopControl::controlDelete(
  void* source)
{
  if (m_keyFocus == source)
    m_keyFocus = NULL;
    
  if (m_mouseTarget == source)
    m_mouseTarget = NULL;

  if (m_mouseFocus == source)
    m_mouseFocus = NULL;
}

//--------------------------------------------------------------
// listener for control add child
void mgTopControl::controlAddChild(
  void* source)
{
}

//--------------------------------------------------------------
// listener for control remove child
void mgTopControl::controlRemoveChild(
  void* source)
{
}

#ifdef WORKED
//--------------------------------------------------------------
// inform topcontrol of cursor change
void mgTopControl::cursorChanged(
  mgControl* source,
  const mgCursor* cursor)
{
  if (m_mouseTarget == source)
    m_surface->setCursorPattern(cursor);
}
#endif
