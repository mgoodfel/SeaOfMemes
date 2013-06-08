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

#include "GUI/mgToolkit.h"
#include "SimpleStyle/mgSimpleWindow.h"
#include "SimpleStyle/mgSimpleDesktop.h"
#include "SimpleStyle/mgSimpleStyle.h"

//--------------------------------------------------------------
// constructor
mgSimpleDesktop::mgSimpleDesktop(
  mgControl* parent,
  const char* cntlName)
: mgDesktopControl(parent, cntlName)
{
  addMouseListener(this);
  addTimeListener(this);  
}

//--------------------------------------------------------------
// destructor
mgSimpleDesktop::~mgSimpleDesktop()
{
  for (int i = 0; i < m_windows.length(); i++)
  {
    mgSimpleWindow* window = (mgSimpleWindow*) m_windows.getAt(i);
    removeChild(window);
    delete window;
  }
  m_windows.removeAll();
}

//--------------------------------------------------------------
// add a window, return pane control
mgControl* mgSimpleDesktop::addWindow(
  const char* windowName)
{
  mgSimpleWindow* window = new mgSimpleWindow(this, this, windowName);
  window->setWindowLocation(MG_WINDOW_ORIGIN_TL, 10+100*m_windows.length(), 10+100*m_windows.length());
  window->setVisible(false);

  m_windows.add(window);
  return window->m_contentPane;
}

//--------------------------------------------------------------
// remove and delete a window
void mgSimpleDesktop::removeWindow(
  mgControl* contentPane)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return;

  BOOL wasActive = windowIsActive(contentPane);

  // repaint after window destroyed
  mgRectangle bounds;
  window->getBounds(bounds);

  // we can't delete the window here, since we might be called
  // from within a handler on the window itself.
  window->setVisible(false);
  m_windows.remove(window);
  m_deleted.add(window);

  damage(bounds);

  // if was active window, update new active window
  if (wasActive && m_windows.length() > 0)
  {
    mgSimpleWindow* top = (mgSimpleWindow*) m_windows.getAt(0);
    top->getBounds(bounds);
    damage(bounds);
  }
}

//--------------------------------------------------------------
// layout the window to preferred size
void mgSimpleDesktop::layoutWindow(
  mgControl* contentPane)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return;

  window->initContentSize();

  mgDimension desktopSize;
  getSize(desktopSize);

  mgRectangle bounds;
  window->requestedBounds(desktopSize, bounds);
  window->setBounds(bounds);
}

//--------------------------------------------------------------
// show the window
void mgSimpleDesktop::showWindow(
  mgControl* contentPane)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return;

  mgDimension contentSize;
  window->getContentSize(contentSize);

  // if first show
  if (contentSize.m_width == 0)
    window->initContentSize();

  mgDimension desktopSize;
  getSize(desktopSize);

  mgRectangle bounds;
  window->requestedBounds(desktopSize, bounds);
  window->setBounds(bounds);

  surfaceWindow(contentPane);
  window->setVisible(true);
}

//--------------------------------------------------------------
// hide the window without deleting it
void mgSimpleDesktop::hideWindow(
  mgControl* contentPane)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return;

  window->setVisible(false);
}

//--------------------------------------------------------------
// surface window
void mgSimpleDesktop::surfaceWindow(
  mgControl* contentPane)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return;

  mgSimpleWindow* oldTop = (mgSimpleWindow*) m_windows.getAt(0);
  if (window != oldTop)
  {
    window->raiseToTop();  // first child
    m_windows.remove(window);
    m_windows.insertAt(0, window);  // first in list
    oldTop->damage();
  }
}

//--------------------------------------------------------------
// set window flags
void mgSimpleDesktop::enableWindowFeatures(
  mgControl* contentPane,
  DWORD flags)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return;

  window->setFlags(flags);
}

//--------------------------------------------------------------
// set window title
void mgSimpleDesktop::setWindowTitle(
  mgControl* contentPane,
  const char* title)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return;

  window->setTitle(title);
}

//--------------------------------------------------------------
// set window location 
void mgSimpleDesktop::setWindowLocation(
  mgControl* contentPane,
  DWORD origin,
  int offsetX,
  int offsetY)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return;

  window->setWindowLocation(origin, offsetX, offsetY);
}

//--------------------------------------------------------------
// set content location 
void mgSimpleDesktop::setContentLocation(
  mgControl* contentPane,
  DWORD origin,
  int offsetX,
  int offsetY)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return;

  window->setContentLocation(origin, offsetX, offsetY);
}

//--------------------------------------------------------------
// set content pane size
void mgSimpleDesktop::setContentSize(
  mgControl* contentPane,
  int width,
  int height)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return;

  window->setContentSize(width, height);
}
              
//--------------------------------------------------------------
// add a desktop listener
void mgSimpleDesktop::addDesktopListener(
  mgDesktopListener* listener)
{
  m_desktopListeners.add(listener);
}

//--------------------------------------------------------------
// remove a desktop listener
void mgSimpleDesktop::removeDesktopListener(
  mgDesktopListener* listener)
{
  m_desktopListeners.remove(listener);
}

//--------------------------------------------------------------
// close-button hit on window
void mgSimpleDesktop::windowClose(
  mgControl* contentPane)
{
  for (int i = 0; i < m_desktopListeners.length(); i++)
  {
    mgDesktopListener* listener = (mgDesktopListener*) m_desktopListeners.getAt(i);
    listener->desktopClose(contentPane);
  }
}

//--------------------------------------------------------------
// maximize-button hit on window
void mgSimpleDesktop::windowMaximize(
  mgControl* contentPane)
{
  for (int i = 0; i < m_desktopListeners.length(); i++)
  {
    mgDesktopListener* listener = (mgDesktopListener*) m_desktopListeners.getAt(i);
    listener->desktopMaximize(contentPane);
  }
}

//--------------------------------------------------------------
// minimize-button hit on window
void mgSimpleDesktop::windowMinimize(
  mgControl* contentPane)
{
  for (int i = 0; i < m_desktopListeners.length(); i++)
  {
    mgDesktopListener* listener = (mgDesktopListener*) m_desktopListeners.getAt(i);
    listener->desktopMinimize(contentPane);
  }
}

//--------------------------------------------------------------
// window was moved
void mgSimpleDesktop::windowMove(
  mgControl* contentPane)
{
  for (int i = 0; i < m_desktopListeners.length(); i++)
  {
    mgDesktopListener* listener = (mgDesktopListener*) m_desktopListeners.getAt(i);
    listener->desktopMove(contentPane);
  }
}

//--------------------------------------------------------------
// window was resized
void mgSimpleDesktop::windowResize(
  mgControl* contentPane)
{
  for (int i = 0; i < m_desktopListeners.length(); i++)
  {
    mgDesktopListener* listener = (mgDesktopListener*) m_desktopListeners.getAt(i);
    listener->desktopResize(contentPane);
  }
}

//--------------------------------------------------------------
// return true if active window
BOOL mgSimpleDesktop::windowIsActive(
  mgControl* contentPane)
{
  mgSimpleWindow* window = findWindow(contentPane);
  if (window == NULL)
    return false;

  for (int i = 0; i < m_windows.length(); i++)
  {
    mgSimpleWindow* top = (mgSimpleWindow*) m_windows.getAt(i);
    if (top->getVisible()) 
    {
      // if topmost visible window is requested window, then it's active
      return window->m_contentPane == contentPane;
    }
  }
  return false;
}

//--------------------------------------------------------------
// find the window by its content pane
mgSimpleWindow* mgSimpleDesktop::findWindow(
  mgControl* contentPane)
{
  for (int i = 0; i < m_windows.length(); i++)
  {
    mgSimpleWindow* window = (mgSimpleWindow*) m_windows.getAt(i);
    if (window->m_contentPane == contentPane)
      return window;
  }
  return NULL;
}

//--------------------------------------------------------------
// update layout of controls
void mgSimpleDesktop::updateLayout()
{
  mgDimension desktopSize;
  getSize(desktopSize);

  for (int i = 0; i < m_windows.length(); i++)
  {
    mgSimpleWindow* window = (mgSimpleWindow*) m_windows.getAt(i);

    mgRectangle windowBounds;
    window->requestedBounds(desktopSize, windowBounds);

    // constrain size to fit on desktop
    windowBounds.m_width = min(windowBounds.m_width, desktopSize.m_width - windowBounds.m_x);
    windowBounds.m_height = min(windowBounds.m_height, desktopSize.m_height - windowBounds.m_y);

    window->setBounds(windowBounds);
  }
}

//--------------------------------------------------------------
// paint content of control
void mgSimpleDesktop::paint(
  mgContext* gc) 
{
  mgDimension size;
  getSize(size);

  gc->setAlphaMode(MG_ALPHA_SET);
  gc->setPen(getSurface()->createPen(2, 1, 0, 0));
  gc->drawRect(1, 1, size.m_width-2, size.m_height-2); 
}

//--------------------------------------------------------------
// get minimum size
void mgSimpleDesktop::minimumSize(
  mgDimension& size) 
{
  preferredSize(size);
}

//--------------------------------------------------------------
// get preferred size
void mgSimpleDesktop::preferredSize(
  mgDimension& size) 
{
#ifdef WORKED
  int iconSpacing = m_iconHeight/3;

  size.m_height = m_displayLines * m_lineHeight;
  size.m_width = 8*m_font->stringWidth("n", 1);  // min width
  
  for (int i = 0; i < m_entries.length(); i++)
  {
    mgListEntry* entry = (mgListEntry*) m_entries.getAt(i);

    if (!entry->m_text.isEmpty())
      size.m_width = max(size.m_width, m_font->stringWidth(entry->m_text, entry->m_text.length()));
  }
  // add room for icon
  if (m_hasIcons)
    size.m_width += iconSpacing + m_iconHeight + iconSpacing;

  if (m_itemUpFrame != NULL)
  {
    mgDimension outside;
    m_itemUpFrame->getOutsideSize(size, outside);
    // frame height already added to m_lineHeight, so just take width
    size.m_width = outside.m_width;
  }

  // add cntl frame
  if (m_upFrame != NULL)
  {
    mgDimension outside;
    m_upFrame->getOutsideSize(size, outside);
    size = outside;
  }

  // add scrollbars
  mgDimension vertSize, horzSize;
  if (m_vertScroller != NULL)
  {
    m_vertScroller->preferredSize(vertSize);
    size.m_width += vertSize.m_width;
  }

  if (m_horzScroller != NULL)
  {
    m_horzScroller->preferredSize(horzSize);
    size.m_height += horzSize.m_height;
  }
#endif
}

//--------------------------------------------------------------
// mouse entered
void mgSimpleDesktop::mouseEnter(
  void* source,
  int x,
  int y)
{
#ifdef WORKED
  mouseMove(source, x, y, 0);
  damage();
#endif
}

//--------------------------------------------------------------
// mouse exited
void mgSimpleDesktop::mouseExit(
  void* source)
{
#ifdef WORKED
  m_hover = -1;
  damage();
#endif
}

//--------------------------------------------------------------
// mouse pressed
void mgSimpleDesktop::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
#ifdef WORKED
//  takeKeyFocus();

  mgDimension size;
  getSize(size);

  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (m_upFrame != NULL)
    m_upFrame->getInsideRect(inside);

  // =-= select entry under point
  y = y - inside.m_y;

  int index = (y/m_lineHeight) + m_scrollPosn;
  if (index < m_entries.length())
  {
    if (m_multiSelect)
    {
      // =-= shift select, cntl select, start of drag select
      // =-= reset all other selections if just click
      mgListEntry* entry = (mgListEntry*) m_entries.getAt(index);
      entry->m_selected = true;
    }
    else
    {
      m_selected = index;
    }
    damage();
  }
#endif
}

//--------------------------------------------------------------
// mouse released
void mgSimpleDesktop::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
#ifdef WORKED
  // =-= if selection changed, notify listeners
  if (m_multiSelect)
  {
  }
  else
  {
    if (m_selected != -1)
    {
      // =-= should deselect old value
      mgListEntry* entry = (mgListEntry*) m_entries.getAt(m_selected);
      //=-= sendSelectEvent(mgSelectOnEvent, entry->m_name);
    }
  }
#endif
}

//--------------------------------------------------------------
// mouse clicked
void mgSimpleDesktop::mouseClick(
  void* source,
  int x,
  int y,
  int modifiers,
  int button,
  int clickCount)
{
#ifdef WORKED
  mouseDown(source, x, y, modifiers, button);
#endif
}

//--------------------------------------------------------------
// mouse dragged
void mgSimpleDesktop::mouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
#ifdef WORKED
  // =-= select entry under point
  // =-= drag select
#endif
}

//--------------------------------------------------------------
// mouse moved
void mgSimpleDesktop::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
#ifdef WORKED
  mgDimension size;
  getSize(size);

  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (m_upFrame != NULL)
    m_upFrame->getInsideRect(inside);

  // find entry under point
  y = y - inside.m_y;

  int index = (y/m_lineHeight) + m_scrollPosn;
  if (index < m_entries.length())
  {
    m_hover = index;
    damage();
  }
  else m_hover = -1;
#endif
}

//--------------------------------------------------------------
// time has passed
void mgSimpleDesktop::guiTimerTick(
  double now,
  double since)
{
  // clean up any deleted windows
  for (int i = m_deleted.length()-1; i >= 0; i--)
  {
    mgSimpleWindow* window = (mgSimpleWindow*) m_deleted.getAt(i);
    m_deleted.removeAt(i);

    removeChild(window);
    delete window;
  }
}
