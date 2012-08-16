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

#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

// Superclass of all Toolkit controls

#include "GUI/mgLayoutManager.h"

#include "GUI/mgControlListener.h"
#include "GUI/mgMouseListener.h"
#include "GUI/mgKeyListener.h"
#include "GUI/mgFocusListener.h"

#include "GUI/mgControl.h"

//--------------------------------------------------------------
// constructor
mgControl::mgControl(
  mgControl* parent,
  const char* cntlName)
{
  m_x = 0;
  m_y = 0;
  m_width = 0;
  m_height = 0;
  
  m_parent = parent;
  m_children = NULL;
  m_layout = NULL;
//  m_cursor = NULL;
  m_cntlName = cntlName;
  
  m_controlListeners = NULL;
  m_mouseListeners = NULL;
  m_keyListeners = NULL;
  m_focusListeners = NULL;
  
  m_enabled = true;
  m_visible = true;

  if (m_parent != NULL)
    m_parent->addChild(this);
}

//--------------------------------------------------------------
// destructor
mgControl::~mgControl() 
{
  // delete all our children
  if (m_children != NULL)
  {
    for (int i = 0; i < m_children->length(); i++)
    {
      mgControl* child = (mgControl*) m_children->getAt(i);
      delete child;
    }
    delete m_children;
    m_children = NULL;
  }
  
  // notify listeners we're being deleted
  dispatchControlDelete(this);

  // delete all the listeners
  delete m_controlListeners;
  m_controlListeners = NULL;
  
  delete m_mouseListeners;
  m_mouseListeners = NULL;
  
  delete m_keyListeners;
  m_keyListeners = NULL;
  
  delete m_focusListeners;
  m_focusListeners = NULL;
  
  // delete the layout
  delete m_layout;
  m_layout = NULL;
  
  m_parent = NULL;
}

//--------------------------------------------------------------
// set the name
void mgControl::setName(
  const char* cntlName)
{
  m_cntlName = cntlName;
}

//--------------------------------------------------------------
// get the name
void mgControl::getName(
  mgString& cntlName) const
{
  if (m_cntlName.isEmpty())
    cntlName = "unnamed";
  else cntlName = m_cntlName;
}
  
//--------------------------------------------------------------
// return parent control
mgControl* mgControl::getParent() const
{
  return m_parent;
}

//--------------------------------------------------------------
// return control to use as parent for menu panes, other popups
mgControl* mgControl::getPage()
{
  if (m_parent != NULL)
    return m_parent->getPage();
  else return NULL;
}
  
//--------------------------------------------------------------
// return layout manager
mgLayoutManager* mgControl::getLayout()
{
  return m_layout;
}
  
//--------------------------------------------------------------
// return UI style 
mgStyle* mgControl::getStyle()
{
  // top control should answer this 
  if (m_parent != NULL)
    return m_parent->getStyle();
  else return NULL;
}
  
//--------------------------------------------------------------
// get control location
void mgControl::getLocation(
  int& x,
  int& y) const
{
  x = m_x;
  y = m_y;
}
  
//--------------------------------------------------------------
// get control size
void mgControl::getSize(
  int& width,
  int& height) const
{
  width = m_width;
  height = m_height;
}

//--------------------------------------------------------------
// get control size
void mgControl::getSize(
  mgDimension& size)
{
  size.m_width = m_width;
  size.m_height = m_height;
}
  
//--------------------------------------------------------------
// set control bounds
void mgControl::setBounds(
  const mgRectangle& bounds)
{
  setLocation(bounds.m_x, bounds.m_y);
  setSize(bounds.m_width, bounds.m_height);
}
  
//--------------------------------------------------------------
// get control bounds
void mgControl::getBounds(
  mgRectangle& bounds)
{
  bounds.m_x = m_x;
  bounds.m_y = m_y;
  bounds.m_width = m_width;
  bounds.m_height = m_height;
}
  
//--------------------------------------------------------------
// get child count
int mgControl::childCount() const
{
  if (m_children == NULL)
    return 0;
  return m_children->length();
}
  
//--------------------------------------------------------------
// get nth child
mgControl* mgControl::getChild(
  int index) const
{
  if (m_children == NULL)
    return NULL;
  return (mgControl*) m_children->getAt(index);
}

//--------------------------------------------------------------
// damage rectangle within control (convenience, don't subclass)
void mgControl::damage(
  const mgRectangle& bounds)
{
  damage(bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height);
}
  
//--------------------------------------------------------------
// damage entire rectangle of control (convenience, don't subclass)
void mgControl::damage()
{
  damage(0, 0, m_width, m_height);
}
  
//--------------------------------------------------------------
// paint content of control
void mgControl::paint(
  mgContext* gc) 
{
}

//--------------------------------------------------------------
// set control location
void mgControl::setLocation(
  int x,
  int y) 
{
  if (x == m_x && y == m_y)
    return;  // nothing to do

  damage();
  m_x = x;
  m_y = y;

  // signal listeners
  dispatchControlMove(this);

  damage();
}

//--------------------------------------------------------------
// set control size
void mgControl::setSize(
  int width,
  int height) 
{
  if (m_width == width && m_height == height)
    return;  // nothing to do

  damage();
  m_width = width;
  m_height = height;

  // adjust layout
  updateLayout();

  // signal listeners
  dispatchControlResize(this);

  damage();
}

//--------------------------------------------------------------
// set layout manager to use
void mgControl::setLayout(
  mgLayoutManager* layout)
{
  // delete previous layout, if any
  if (m_layout != layout)
    delete m_layout;

  // we own new layout
  m_layout = layout;

// =-= too early to change this during layout constructor.  should 
// be called if changing layout on existing control.
//  updateLayout();
}

//--------------------------------------------------------------
// force update of layout
void mgControl::updateLayout()
{
  if (m_layout != NULL)
    m_layout->controlResized();
}

//--------------------------------------------------------------
// damage rectangle within control
void mgControl::damage(
  int x,
  int y,
  int width,
  int height) 
{
  if (m_parent != NULL)
    m_parent->damage(x+m_x, y+m_y, width, height);
}

//--------------------------------------------------------------
// send show/hide notifications for children
void mgControl::notifyVisible(
  BOOL show)
{
  if (m_children != NULL)
  {
    for (int i = 0; i < m_children->length(); i++)
    {
      mgControl* child = (mgControl*) m_children->getAt(i);
      if (child->m_visible)
      {
        if (show)
          dispatchControlShow(child);
        else dispatchControlHide(child);
      
        child->notifyVisible(show);
      }
    }
  }
}

//--------------------------------------------------------------
// set visible flag
void mgControl::setVisible(
  BOOL visible) 
{
  BOOL wasVisible = getVisible();
  // if no change, do nothing
  if (m_visible == visible)
    return;
  m_visible = visible;
  
  // if hiding control
  if (wasVisible && !m_visible)
  {
    // if the key focus has become invisible
    mgControl* keyFocus = getKeyFocus();
    if (keyFocus != NULL && isAncestorOf(keyFocus))
      setKeyFocus(NULL);

    damage();
    dispatchControlHide(this);

    // hide all the visible children
    notifyVisible(false);
  }

  // if becoming visible
  else if (!wasVisible && getVisible())
  {
    damage();
    dispatchControlShow(this);

    // show all the visible children
    notifyVisible(true);
  }
}

//--------------------------------------------------------------
// return visible flag
BOOL mgControl::getVisible() 
{
  if (m_parent != NULL)
    return m_visible && m_parent->getVisible();
  else return m_visible;
}

//--------------------------------------------------------------
// set enabled flag
void mgControl::setEnabled(
  BOOL enabled) 
{
  BOOL wasEnabled = getEnabled();
  m_enabled = enabled;
  
  // notify listeners if enable status changed
  if (wasEnabled != getEnabled())
  {
    // lose the key focus when disabled
    if (!m_enabled)
      releaseKeyFocus();
      
    damage();
    if (m_enabled)
      dispatchControlEnable(this);
    else dispatchControlDisable(this);

    // set all the children to current enabled status.  should cause event to be sent
    if (m_children != NULL)
    {
      for (int i = 0; i < m_children->length(); i++)
      {
        mgControl* child = (mgControl*) m_children->getAt(i);
        child->setEnabled(child->m_enabled);
      }
    }
  }
}

//--------------------------------------------------------------
// get enabled flag
BOOL mgControl::getEnabled() 
{
  if (m_parent != NULL)
    return m_enabled && m_parent->getEnabled();
  else return m_enabled;
}

//--------------------------------------------------------------
// add a child to top of stack
void mgControl::addChild(
  mgControl* child) 
{
  // remove from existing parent
  if (child->m_parent != NULL)
    child->m_parent->removeChild(child);

  // add to child list
  if (m_children == NULL)
    m_children = new mgPtrArray();
  m_children->add(child);
  child->m_parent = this;
  
  // damage its bounds
  mgRectangle bounds;
  child->getBounds(bounds);
  damage(bounds);
}

//--------------------------------------------------------------
// add child to bottom of stack
void mgControl::addChildToBottom(
  mgControl* child) 
{
  if (m_children == NULL)
    m_children = new mgPtrArray();
  m_children->insertAt(0, child);
  
  child->m_parent = this;

  mgRectangle bounds;
  child->getBounds(bounds);
  damage(bounds);
}

//--------------------------------------------------------------
// remove a child
void mgControl::removeChild(
  mgControl* child) 
{
  if (m_children == NULL)
    m_children = new mgPtrArray();
  m_children->remove(child);

  mgRectangle bounds;
  child->getBounds(bounds);
  damage(bounds);

  child->m_parent = NULL;
}

//--------------------------------------------------------------
// raise to top of siblings
void mgControl::raiseToTop() 
{
  if (m_parent != NULL)
  {
    m_parent->m_children->remove(this);
    m_parent->m_children->add(this);
    damage();
  }
}

//--------------------------------------------------------------
// sink to bottom of siblings
void mgControl::sinkToBottom() 
{
  if (m_parent != NULL)
  {
    m_parent->removeChild(this);
    m_parent->addChildToBottom(this);
  }
}

//--------------------------------------------------------------
// get a graphics context to draw with
mgContext* mgControl::newContext()
{
  if (m_parent != NULL)
  {
    mgContext* gc = m_parent->newContext();
    gc->translate(m_x, m_y);
    gc->setClip(0, 0, m_width, m_height);
    return gc;
  }
  else return NULL;  // unparented control
}

//--------------------------------------------------------------
// get rendering surface
mgSurface* mgControl::getSurface()
{
  if (m_parent != NULL)
    return m_parent->getSurface();

  else return NULL;  // unparented control
}

#ifdef WORKED
//--------------------------------------------------------------
// set cursor to use for this control
void mgControl::setCursor(
  const mgCursor* cursor)
{
  m_cursor = cursor;
  // inform parent of cursor change
  if (m_parent != NULL)
    m_parent->cursorChanged(this, cursor);
}
  
//--------------------------------------------------------------
// inform topcontrol of cursor change
void mgControl::cursorChanged(
  mgControl* source,
  const mgCursor* cursor)
{
  if (m_parent != NULL)
    m_parent->cursorChanged(source, cursor);
}

//--------------------------------------------------------------
// get cursor pattern for this control
const mgCursor* mgControl::getCursor()
{
  // if we don't set pattern, get parent
  if (m_cursor != NULL)
    return m_cursor;
  else if (m_parent != NULL)
    return m_parent->getCursor();
  else return NULL;
}
#endif

//--------------------------------------------------------------
// repaint background
void mgControl::paintBackground(
  mgContext* gc)
{
  if (m_layout != NULL)
    m_layout->paintBackground(gc);
}

//--------------------------------------------------------------
// repaint foreground
void mgControl::paintForeground(
  mgContext* gc)
{
  if (m_layout != NULL)
    m_layout->paintForeground(gc);
}

//--------------------------------------------------------------
// repaint children of control
void mgControl::paintChildren(
  mgContext* gc,
  int clipLeft,
  int clipTop,
  int clipRight,
  int clipBottom) 
{
  if (m_children == NULL)
    return;

  // for each child, bottom to top, paint
  for (int i = 0; i < m_children->length(); i++)
  {
    mgControl* child = (mgControl*) m_children->getAt(i);
    if (!child->m_visible)
      continue;

    // set the clip and origin for each child
    int childLeft = max(clipLeft, child->m_x);
    int childTop = max(clipTop, child->m_y);
    int childRight = min(clipRight, child->m_x + child->m_width);
    int childBottom = min(clipBottom, child->m_y + child->m_height);
    if (childLeft < childRight && childTop < childBottom)
    {
//      mgDebug("paintChild (%08x) intersect = (%d, %d) %d by %d", 
//              this, childLeft, childTop, childRight-childLeft, childBottom-childTop);
      void* state = gc->getState();
      gc->setClip(childLeft, childTop, childRight-childLeft, childBottom-childTop);
      gc->translate(child->m_x, child->m_y);

      child->paintControl(gc, childLeft - child->m_x, childTop - child->m_y,
                             childRight - child->m_x, childBottom - child->m_y);

      gc->setState(state);
      gc->deleteState(state);
    }
  }
}

//--------------------------------------------------------------
// paint control and all children
void mgControl::paintControl(
  mgContext* gc,
  int clipLeft,
  int clipTop,
  int clipRight,
  int clipBottom) 
{
  // restore state after control methods paint
  void* state = gc->getState();

  paintBackground(gc);
  gc->setState(state);

  paint(gc);
  gc->setState(state);

  paintChildren(gc, clipLeft, clipTop, clipRight, clipBottom);

  gc->setState(state);
  paintForeground(gc);

  gc->deleteState(state);
}

//--------------------------------------------------------------
// return min size of control
void mgControl::minimumSize(
  mgDimension& size)
{
  if (m_layout != NULL)
    m_layout->minimumSize(size);
  else 
  {
    size.m_width = 0;
    size.m_height = 0;
  }
}

//--------------------------------------------------------------
// return preferred size of control
void mgControl::preferredSize(
  mgDimension& size)
{
  if (m_layout != NULL)
    m_layout->preferredSize(size);
  else minimumSize(size);
}

//--------------------------------------------------------------
// return true if control is ancestor of arg
BOOL mgControl::isAncestorOf(
  mgControl* cntl)
{
  while (cntl != NULL)
  {
    if (cntl == this)
      return true;
    cntl = cntl->m_parent;
  }
  return false;
}

//--------------------------------------------------------------
// compute size at width.  return false if not implemented
BOOL mgControl::preferredSizeAtWidth(
  int width,
  mgDimension& size)
{
  if (m_layout != NULL)
    return m_layout->preferredSizeAtWidth(width, size);
  else return false;
}

//--------------------------------------------------------------
// convert local coordinates to topControl coordinates
void mgControl::getLocationInTop(
  mgPoint& point)
{
  if (m_parent != NULL)
  {
    m_parent->getLocationInTop(point);
    point.m_x += m_x;
    point.m_y += m_y;
  }
}

//--------------------------------------------------------------
// convert local coordinates to ancestor coordinates
void mgControl::getLocationInAncestor(
  mgControl* ancestor,
  mgPoint& point)
{
  if (m_parent != ancestor && m_parent != NULL)
    m_parent->getLocationInAncestor(ancestor, point);

  point.m_x += m_x;
  point.m_y += m_y;
}

//--------------------------------------------------------------
// convert local coordinates to ancestor coordinates
void mgControl::getLocationInAncestor(
  mgControl* ancestor,
  mgRectangle& rect)
{
  if (m_parent != ancestor && m_parent != NULL)
    m_parent->getLocationInAncestor(ancestor, rect);

  rect.m_x += m_x;
  rect.m_y += m_y;
}

//--------------------------------------------------------------
// find control under point
mgControl* mgControl::findControlAtPoint(
  int x,
  int y)
{
  if (m_children == NULL)
    return this;

  // from top to bottom
  for (int i = m_children->length()-1; i >= 0; i--)
  {
    mgControl* child = (mgControl*) m_children->getAt(i);
    if (!child->m_visible)
      continue;

    if (child->m_x <= x && child->m_x+child->m_width > x &&
        child->m_y <= y && child->m_y+child->m_height > y)
      return child->findControlAtPoint(x - child->m_x, y - child->m_y);
  }
  
  // if none found, return this control
  return this;
}

//--------------------------------------------------------------
// return true if control accepts key focus
BOOL mgControl::acceptsKeyFocus()
{
  return false;
}

//--------------------------------------------------------------
// take the key focus
void mgControl::takeKeyFocus()
{
  setKeyFocus(this);
}

//--------------------------------------------------------------
// release the key focus
void mgControl::releaseKeyFocus()
{
  if (isKeyFocus())
  {
    setKeyFocus(NULL);
  }
}

//--------------------------------------------------------------
// return true if control is key focus
BOOL mgControl::isKeyFocus()
{
  return getKeyFocus() == this;
}

//--------------------------------------------------------------
// set the key focus
void mgControl::setKeyFocus(
  mgControl* focus)
{
  // work up the tree to the top control
  if (m_parent != NULL)
    m_parent->setKeyFocus(focus);
}

//--------------------------------------------------------------
// return the key focus
mgControl* mgControl::getKeyFocus()
{
  if (m_parent != NULL)
    return m_parent->getKeyFocus();
  else return NULL;
}

//--------------------------------------------------------------
// return the mouse focus
mgControl* mgControl::getMouseFocus()
{
  if (m_parent != NULL)
    return m_parent->getMouseFocus();
  else return NULL;
}

//--------------------------------------------------------------
// return true if control is mouse focus
BOOL mgControl::isMouseFocus()
{
  return getMouseFocus() == this;
}

//--------------------------------------------------------------
// add a control listener
void mgControl::addControlListener(
  mgControlListener* listener)
{
  if (m_controlListeners == NULL)
    m_controlListeners = new mgPtrArray();
  m_controlListeners->add(listener);
}

//--------------------------------------------------------------
// remove a control listener
void mgControl::removeControlListener(
  mgControlListener* listener)
{
  if (m_controlListeners == NULL)
    return;
  m_controlListeners->remove(listener);
}

//--------------------------------------------------------------
// send resize to control listeners
void mgControl::dispatchControlResize(
  void* source)
{
  if (m_controlListeners == NULL)
    return;
  
  for (int i = 0; i < m_controlListeners->length(); i++)
  {
    mgControlListener* listener = (mgControlListener*) m_controlListeners->getAt(i);
    listener->controlResize(source);
  }
}

//--------------------------------------------------------------
// send move to control listeners
void mgControl::dispatchControlMove(
  void* source)
{
  if (m_controlListeners == NULL)
    return;
  
  for (int i = 0; i < m_controlListeners->length(); i++)
  {
    mgControlListener* listener = (mgControlListener*) m_controlListeners->getAt(i);
    listener->controlMove(source);
  }
}

//--------------------------------------------------------------
// send show to control listeners
void mgControl::dispatchControlShow(
  void* source)
{
  if (m_controlListeners == NULL)
    return;
  
  for (int i = 0; i < m_controlListeners->length(); i++)
  {
    mgControlListener* listener = (mgControlListener*) m_controlListeners->getAt(i);
    listener->controlShow(source);
  }
}

//--------------------------------------------------------------
// send hide to control listeners
void mgControl::dispatchControlHide(
  void* source)
{
  if (m_controlListeners == NULL)
    return;
  
  for (int i = 0; i < m_controlListeners->length(); i++)
  {
    mgControlListener* listener = (mgControlListener*) m_controlListeners->getAt(i);
    listener->controlHide(source);
  }
}

//--------------------------------------------------------------
// send enable to control listeners
void mgControl::dispatchControlEnable(
  void* source)
{
  if (m_controlListeners == NULL)
    return;
  
  for (int i = 0; i < m_controlListeners->length(); i++)
  {
    mgControlListener* listener = (mgControlListener*) m_controlListeners->getAt(i);
    listener->controlEnable(source);
  }
}

//--------------------------------------------------------------
// send disable to control listeners
void mgControl::dispatchControlDisable(
  void* source)
{
  if (m_controlListeners == NULL)
    return;
  
  for (int i = 0; i < m_controlListeners->length(); i++)
  {
    mgControlListener* listener = (mgControlListener*) m_controlListeners->getAt(i);
    listener->controlDisable(source);
  }
}

//--------------------------------------------------------------
// send delete to control listeners
void mgControl::dispatchControlDelete(
  void* source)
{
  if (m_controlListeners == NULL)
    return;
  
  for (int i = 0; i < m_controlListeners->length(); i++)
  {
    mgControlListener* listener = (mgControlListener*) m_controlListeners->getAt(i);
    listener->controlDelete(source);
  }
}

//--------------------------------------------------------------
// add a mouse listener
void mgControl::addMouseListener(
  mgMouseListener* listener)
{
  if (m_mouseListeners == NULL)
    m_mouseListeners = new mgPtrArray();
  m_mouseListeners->add(listener);
}

//--------------------------------------------------------------
// remove a mouse listener
void mgControl::removeMouseListener(
  mgMouseListener* listener)
{
  if (m_mouseListeners == NULL)
    return;
  m_mouseListeners->remove(listener);
}

//--------------------------------------------------------------
// send enter to mouse listeners
void mgControl::dispatchMouseEnter(
  void* source,
  int x,
  int y)
{
  if (m_mouseListeners == NULL)
    return;

  for (int i = 0; i < m_mouseListeners->length(); i++)
  {
    mgMouseListener* listener = (mgMouseListener*) m_mouseListeners->getAt(i);
    listener->mouseEnter(source, x, y);
  }
}

//--------------------------------------------------------------
// send exited to mouse listeners
void mgControl::dispatchMouseExit(
  void* source)
{
  if (m_mouseListeners == NULL)
    return;

  for (int i = 0; i < m_mouseListeners->length(); i++)
  {
    mgMouseListener* listener = (mgMouseListener*) m_mouseListeners->getAt(i);
    listener->mouseExit(source);
  }
}

//--------------------------------------------------------------
// send down to mouse listeners
void mgControl::dispatchMouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  if (m_mouseListeners == NULL)
    return;

  for (int i = 0; i < m_mouseListeners->length(); i++)
  {
    mgMouseListener* listener = (mgMouseListener*) m_mouseListeners->getAt(i);
    listener->mouseDown(source, x, y, modifiers, button);
  }
}

//--------------------------------------------------------------
// send up to mouse listeners
void mgControl::dispatchMouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  if (m_mouseListeners == NULL)
    return;

  for (int i = 0; i < m_mouseListeners->length(); i++)
  {
    mgMouseListener* listener = (mgMouseListener*) m_mouseListeners->getAt(i);
    listener->mouseUp(source, x, y, modifiers, button);
  }
}

//--------------------------------------------------------------
// mouse clicked
void mgControl::dispatchMouseClick(
  void* source,
  int x,
  int y,
  int modifiers,
  int button,
  int clickCount)
{
  if (m_mouseListeners == NULL)
    return;

  for (int i = 0; i < m_mouseListeners->length(); i++)
  {
    mgMouseListener* listener = (mgMouseListener*) m_mouseListeners->getAt(i);
    listener->mouseClick(source, x, y, modifiers, button, clickCount);
  }
}

//--------------------------------------------------------------
// mouse dragged
void mgControl::dispatchMouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
  if (m_mouseListeners == NULL)
    return;

  for (int i = 0; i < m_mouseListeners->length(); i++)
  {
    mgMouseListener* listener = (mgMouseListener*) m_mouseListeners->getAt(i);
    listener->mouseDrag(source, x, y, modifiers);
  }
}

//--------------------------------------------------------------
// mouse moved
void mgControl::dispatchMouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
  if (m_mouseListeners == NULL)
    return;

  for (int i = 0; i < m_mouseListeners->length(); i++)
  {
    mgMouseListener* listener = (mgMouseListener*) m_mouseListeners->getAt(i);
    listener->mouseMove(source, x, y, modifiers);
  }
}

//--------------------------------------------------------------
// add a key listener
void mgControl::addKeyListener(
  mgKeyListener* listener)
{
  if (m_keyListeners == NULL)
    m_keyListeners = new mgPtrArray();
  m_keyListeners->add(listener);
}

//--------------------------------------------------------------
// remove a key listener
void mgControl::removeKeyListener(
  mgKeyListener* listener)
{
  if (m_keyListeners == NULL)
    return;
  m_keyListeners->remove(listener);
}

//--------------------------------------------------------------
// send down to key listeners
void mgControl::dispatchKeyDown(
  void* source,
  int key,
  int modifiers)
{
  if (m_keyListeners == NULL)
    return;

  for (int i = 0; i < m_keyListeners->length(); i++)
  {
    mgKeyListener* listener = (mgKeyListener*) m_keyListeners->getAt(i);
    listener->keyDown(source, key, modifiers);
  }
}

//--------------------------------------------------------------
// send up to key listeners
void mgControl::dispatchKeyUp(
  void* source,
  int key,
  int modifiers)
{
  if (m_keyListeners == NULL)
    return;

  for (int i = 0; i < m_keyListeners->length(); i++)
  {
    mgKeyListener* listener = (mgKeyListener*) m_keyListeners->getAt(i);
    listener->keyUp(source, key, modifiers);
  }
}

//--------------------------------------------------------------
// send char to key listeners
void mgControl::dispatchKeyChar(
  void* source,
  int key,
  int modifiers)
{
  if (m_keyListeners == NULL)
    return;

  for (int i = 0; i < m_keyListeners->length(); i++)
  {
    mgKeyListener* listener = (mgKeyListener*) m_keyListeners->getAt(i);
    listener->keyChar(source, key, modifiers);
  }
}

//--------------------------------------------------------------
// add a focus listener
void mgControl::addFocusListener(
  mgFocusListener* listener)
{
  if (m_focusListeners == NULL)
    m_focusListeners = new mgPtrArray();
  m_focusListeners->add(listener);
}

//--------------------------------------------------------------
// remove a focus listener
void mgControl::removeFocusListener(
  mgFocusListener* listener)
{
  if (m_focusListeners == NULL)
    return;
  m_focusListeners->remove(listener);
}

//--------------------------------------------------------------
// send gained to focus listeners
void mgControl::dispatchFocusGained(
  void* source)
{
  if (m_focusListeners == NULL)
    return;

  for (int i = 0; i < m_focusListeners->length(); i++)
  {
    mgFocusListener* listener = (mgFocusListener*) m_focusListeners->getAt(i);
    listener->guiFocusGained(source);
  }
}
    
//--------------------------------------------------------------
// send lost to focus listeners
void mgControl::dispatchFocusLost(
  void* source)
{
  if (m_focusListeners == NULL)
    return;

  for (int i = 0; i < m_focusListeners->length(); i++)
  {
    mgFocusListener* listener = (mgFocusListener*) m_focusListeners->getAt(i);
    listener->guiFocusLost(source);
  }
}

//--------------------------------------------------------------
// add time listener
void mgControl::addTimeListener(
  mgTimeListener* listener)
{
  // handle these in top control
  if (m_parent != NULL)
    m_parent->addTimeListener(listener);
}

//--------------------------------------------------------------
// remove time listener
void mgControl::removeTimeListener(
  mgTimeListener* listener)
{
  // handle these in top control
  if (m_parent != NULL)
    m_parent->removeTimeListener(listener);
}
