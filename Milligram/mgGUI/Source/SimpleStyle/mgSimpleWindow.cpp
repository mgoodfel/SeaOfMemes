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
#include "SimpleStyle/mgSimpleDesktop.h"
#include "SimpleStyle/mgSimpleButton.h"
#include "SimpleStyle/mgSimpleWindow.h"
#include "SimpleStyle/mgSimpleStyle.h"

const int PRESS_NONE        = 0;
const int PRESS_MOVE        = 1;
const int PRESS_RESIZE_T    = 2;
const int PRESS_RESIZE_TR   = 3;
const int PRESS_RESIZE_R    = 4;
const int PRESS_RESIZE_BR   = 5;
const int PRESS_RESIZE_B    = 6;
const int PRESS_RESIZE_BL   = 7;
const int PRESS_RESIZE_L    = 8;
const int PRESS_RESIZE_TL   = 9;

//--------------------------------------------------------------
// constructor
mgSimpleWindow::mgSimpleWindow(
  mgSimpleDesktop* desktop,
  mgControl* parent,
  const char* cntlName)
: mgControl(parent, cntlName)
{
  m_desktop = desktop;

  m_contentPane = new mgControl(this);
  new mgColumnLayout(m_contentPane);

  mgStyle* style = getStyle();

  // assume style has set all our format elements
  style->getFontAttr(MG_STYLE_WINDOW, getName(), "font", m_font);

  style->getFrameAttr(MG_STYLE_WINDOW, getName(), "onFrame", m_onFrame);
  style->getColorAttr(MG_STYLE_WINDOW, getName(), "onColor", m_onColor);

  style->getFrameAttr(MG_STYLE_WINDOW, getName(), "offFrame", m_offFrame);
  style->getColorAttr(MG_STYLE_WINDOW, getName(), "offColor", m_offColor);

  addMouseListener(this);

  m_pressMode = PRESS_NONE;

  m_closeBtn = NULL;
  m_maxBtn = NULL;
  m_minBtn = NULL;

  m_flags = 0;
  m_barHeight = 0;
  m_iconsWidth = 0;
  m_iconsHeight = 0;
}

//--------------------------------------------------------------
// destructor
mgSimpleWindow::~mgSimpleWindow()
{
}

//--------------------------------------------------------------
// set title
void mgSimpleWindow::setTitle(
  const char* title)
{
  m_title = title;

  // repaint the title bar area
  mgRectangle damageRect;
  mgRectangle windowBounds;
  getSize(windowBounds.m_width, windowBounds.m_height);
  damageRect.m_width = windowBounds.m_width;

  m_onFrame->getInsideRect(windowBounds);
  damageRect.m_height = windowBounds.m_y + m_barHeight;
  damage(damageRect);
}

//--------------------------------------------------------------
// set window flags
void mgSimpleWindow::setFlags(
  DWORD flags)
{
  m_flags = flags;

  if (m_flags & MG_WINDOW_HAS_TITLE)
    m_barHeight = m_font->getHeight();
  else m_barHeight = 0;

  m_iconsWidth = 0;
  m_iconsHeight = 0;

  mgDimension btnSize;

  if (m_flags & MG_WINDOW_CAN_CLOSE)
  {
    m_closeBtn = new mgSimpleButton(this, "close");
    m_closeBtn->setLabel("X");
    m_closeBtn->minimumSize(btnSize);
    m_closeBtn->setSize(btnSize);
    m_closeBtn->addActionListener(this);
    m_barHeight = max(m_barHeight, btnSize.m_height);
    m_iconsHeight = max(m_iconsHeight, btnSize.m_height);
    m_iconsWidth += btnSize.m_width;
  }
  else 
  {
    delete m_closeBtn;
    m_closeBtn = NULL;
  }

  if (m_flags & MG_WINDOW_CAN_MAXIMIZE)
  {
    m_maxBtn = new mgSimpleButton(this, "max");
    m_maxBtn->setLabel("O");
    m_maxBtn->minimumSize(btnSize);
    m_maxBtn->setSize(btnSize);
    m_maxBtn->addActionListener(this);
    m_barHeight = max(m_barHeight, btnSize.m_height);
    m_iconsHeight = max(m_iconsHeight, btnSize.m_height);
    m_iconsWidth += btnSize.m_width;
  }
  else
  {
    delete m_maxBtn;
    m_maxBtn = NULL;
  }

  if (m_flags & MG_WINDOW_CAN_MINIMIZE)
  {
    m_minBtn = new mgSimpleButton(this, "min");
    m_minBtn->setLabel("_");
    m_minBtn->minimumSize(btnSize);
    m_minBtn->setSize(btnSize);
    m_minBtn->addActionListener(this);
    m_barHeight = max(m_barHeight, btnSize.m_height);
    m_iconsHeight = max(m_iconsHeight, btnSize.m_height);
    m_iconsWidth += btnSize.m_width;
  }
  else
  {
    delete m_minBtn;
    m_minBtn = NULL;
  }
}

//--------------------------------------------------------------
// return minimum size
void mgSimpleWindow::minimumSize(
  mgDimension& size)
{
  mgDimension contentSize;

  // figure minimum size of content
  m_contentPane->minimumSize(contentSize);

  int minTitleWidth = 10*m_font->stringWidth("n", 1);

  mgDimension innerSize;
  innerSize.m_width = max(contentSize.m_width, m_iconsWidth + minTitleWidth);
  innerSize.m_height = contentSize.m_height + m_barHeight;

  if (m_flags & MG_WINDOW_HAS_FRAME)
    m_onFrame->getOutsideSize(innerSize, size);
  else size = innerSize;
}  

//--------------------------------------------------------------
// return preferred size
void mgSimpleWindow::preferredSize(
  mgDimension& size)
{
  mgDimension contentSize;

  // figure preferred size of content
  m_contentPane->preferredSize(contentSize);

  int minTitleWidth = 10*m_font->stringWidth("n", 1);

  mgDimension innerSize;
  innerSize.m_width = max(contentSize.m_width, m_iconsWidth + minTitleWidth);
  innerSize.m_height = contentSize.m_height + m_barHeight;

  if (m_flags & MG_WINDOW_HAS_FRAME)
    m_onFrame->getOutsideSize(innerSize, size);
  else size = innerSize;
}  

//--------------------------------------------------------------
// set window location relative to origin
void mgSimpleWindow::setWindowLocation(
  int origin,
  int offsetX,
  int offsetY)
{
  m_locationOrigin = origin;
  m_locationBounds.m_x = offsetX;
  m_locationBounds.m_y = offsetY;
  m_locationContent = false;
}

//--------------------------------------------------------------
// set content location relative to origin
void mgSimpleWindow::setContentLocation(
  int origin,
  int offsetX,
  int offsetY)
{
  m_locationOrigin = origin;
  m_locationBounds.m_x = offsetX;
  m_locationBounds.m_y = offsetY;
  m_locationContent = true;
}

//--------------------------------------------------------------
// initialize content size to preferred size
void mgSimpleWindow::initContentSize()
{
  mgDimension contentSize;
  m_contentPane->preferredSize(contentSize);

  // save requested content size
  m_locationBounds.m_width = contentSize.m_width;
  m_locationBounds.m_height = contentSize.m_height;
}

//--------------------------------------------------------------
// set content size
void mgSimpleWindow::setContentSize(
  int width, 
  int height)
{
  // save requested content size
  m_locationBounds.m_width = width;
  m_locationBounds.m_height = height;
}

//--------------------------------------------------------------
// get requested content size
void mgSimpleWindow::getContentSize(
  mgDimension &contentSize) const
{
  contentSize.m_width = m_locationBounds.m_width;
  contentSize.m_height = m_locationBounds.m_height;
}

//--------------------------------------------------------------
// get requested bounds of window
void mgSimpleWindow::requestedBounds(
  mgDimension& desktopSize,
  mgRectangle& windowBounds)
{
  mgDimension locationSize;
  if (m_locationContent)
  {
    // position the content
    locationSize.m_width = m_locationBounds.m_width;
    locationSize.m_height = m_locationBounds.m_height;
  }
  else
  {
    // position the window
    int minTitleWidth = 10*m_font->stringWidth("n", 1);

    mgDimension innerSize;
    innerSize.m_width = max(m_locationBounds.m_width, m_iconsWidth + minTitleWidth);
    innerSize.m_height = m_locationBounds.m_height + m_barHeight;

    if (m_flags & MG_WINDOW_HAS_FRAME)
      m_onFrame->getOutsideSize(innerSize, locationSize);
  }

  windowBounds.m_width = locationSize.m_width;
  windowBounds.m_height = locationSize.m_height;

  switch (m_locationOrigin)
  {
    case MG_WINDOW_ORIGIN_TL:
      // place window top-left relative to desktop top-left
      windowBounds.m_x = m_locationBounds.m_x;
      windowBounds.m_y = m_locationBounds.m_y;
      break;
    case MG_WINDOW_ORIGIN_TC:
      // place window top-center relative to desktop top-center
      windowBounds.m_x = (desktopSize.m_width/2 + m_locationBounds.m_x) - locationSize.m_width/2;
      windowBounds.m_y = m_locationBounds.m_y;
      break;
    case MG_WINDOW_ORIGIN_TR:
      // place window top-right relative to desktop top-right
      windowBounds.m_x = (desktopSize.m_width - m_locationBounds.m_x) - locationSize.m_width;
      windowBounds.m_y = m_locationBounds.m_y;
      break;
    case MG_WINDOW_ORIGIN_BL:
      // place window bottom-left relative to desktop bottom-left
      windowBounds.m_x = m_locationBounds.m_x;
      windowBounds.m_y = (desktopSize.m_height - m_locationBounds.m_y) - locationSize.m_height;
      break;
    case MG_WINDOW_ORIGIN_BC:
      // place window bottom-center relative to desktop bottom-center
      windowBounds.m_x = (desktopSize.m_width/2 + m_locationBounds.m_x) - locationSize.m_width/2;
      windowBounds.m_y = (desktopSize.m_height - m_locationBounds.m_y) - locationSize.m_height;
      break;
    case MG_WINDOW_ORIGIN_BR:
      // place window bottom-right relative to desktop bottom-right
      windowBounds.m_x = (desktopSize.m_width - m_locationBounds.m_x) - locationSize.m_width;
      windowBounds.m_y = (desktopSize.m_height - m_locationBounds.m_y) - locationSize.m_height;
      break;
    case MG_WINDOW_ORIGIN_CC:
      // place window center relative to desktop center
      windowBounds.m_x = (desktopSize.m_width/2 + m_locationBounds.m_x) - locationSize.m_width/2;
      windowBounds.m_y = (desktopSize.m_height/2 + m_locationBounds.m_y) - locationSize.m_height/2;
      break;
    case MG_WINDOW_ORIGIN_CL:
      // place window left-center relative to desktop left-center
      windowBounds.m_x = m_locationBounds.m_x;
      windowBounds.m_y = (desktopSize.m_height/2 + m_locationBounds.m_y) - locationSize.m_height/2;
      break;
    case MG_WINDOW_ORIGIN_CR:
      // place window right-center relative to desktop right-center
      windowBounds.m_x = (desktopSize.m_width - m_locationBounds.m_x) - locationSize.m_width;
      windowBounds.m_y = (desktopSize.m_height/2 + m_locationBounds.m_y) - locationSize.m_height/2;
      break;
  }
}

//--------------------------------------------------------------
// update layout of children
void mgSimpleWindow::updateLayout()
{
  mgRectangle windowBounds;
  getSize(windowBounds.m_width, windowBounds.m_height);

  if (m_flags & MG_WINDOW_HAS_FRAME)
    m_onFrame->getInsideRect(windowBounds);

  mgDimension btnSize;

  int btnX = windowBounds.right();

  if (m_flags & MG_WINDOW_CAN_CLOSE)
  {
    m_closeBtn->getSize(btnSize);
    btnX -= btnSize.m_width;
    m_closeBtn->setLocation(btnX, windowBounds.m_y + (m_barHeight - btnSize.m_height)/2);
  }

  if (m_flags & MG_WINDOW_CAN_MAXIMIZE)
  {
    m_maxBtn->getSize(btnSize);
    btnX -= btnSize.m_width;
    m_maxBtn->setLocation(btnX, windowBounds.m_y + (m_barHeight - btnSize.m_height)/2);
  }

  if (m_flags & MG_WINDOW_CAN_MINIMIZE)
  {
    m_minBtn->getSize(btnSize);
    btnX -= btnSize.m_width;
    m_minBtn->setLocation(btnX, windowBounds.m_y + (m_barHeight - btnSize.m_height)/2);
  }

  m_contentPane->setLocation(windowBounds.m_x, windowBounds.m_y + m_barHeight);
  m_contentPane->setSize(windowBounds.m_width, windowBounds.m_height - m_barHeight);
}
  
//--------------------------------------------------------------
// paint content of control
void mgSimpleWindow::paint(
  mgContext* gc)
{
  BOOL active = m_desktop->windowIsActive(m_contentPane);

  mgDimension size;
  getSize(size);

  mgRectangle inside(0, 0, size.m_width, size.m_height);
  const mgFrame* frame = active ? m_onFrame : m_offFrame;
  if (m_flags & MG_WINDOW_HAS_FRAME)
  {
    frame->paintBackground(gc, 0, 0, size.m_width, size.m_height);
    frame->getInsideRect(inside);
  }

  if (m_flags & MG_WINDOW_HAS_TITLE)
  {
    if (active)
      gc->setTextColor(m_onColor);
    else gc->setTextColor(m_offColor);

    gc->setFont(m_font);
    int titleHeight = m_font->getHeight();

    gc->setAlphaMode(MG_ALPHA_MERGE);
    gc->drawString(m_title, m_title.length(), inside.m_x, inside.m_y + (m_barHeight - titleHeight)/2 + m_font->getAscent());
  }

  if (m_flags & MG_WINDOW_HAS_FRAME)
    frame->paintForeground(gc, 0, 0, size.m_width, size.m_height);
}  

//--------------------------------------------------------------
// action performed
void mgSimpleWindow::guiAction(
  void* source,
  const char* name)
{
  if (source == m_closeBtn)
  {
    // tell desktop to close
    m_desktop->windowClose(m_contentPane);
  }
  else if (source == m_minBtn)
  {
    // tell desktop to minimize
    m_desktop->windowMinimize(m_contentPane);
  }
  else if (source == m_maxBtn)
  {
    // tell desktop to maximize
    m_desktop->windowMaximize(m_contentPane);
  }
}

//--------------------------------------------------------------
// mouse entered
void mgSimpleWindow::mouseEnter(
  void* source,
  int x,
  int y)
{
}

//--------------------------------------------------------------
// mouse exited
void mgSimpleWindow::mouseExit(
  void* source)
{
}

//--------------------------------------------------------------
// mouse pressed
void mgSimpleWindow::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // surface the window
  m_desktop->surfaceWindow(m_contentPane);

  mgRectangle windowBounds;
  getSize(windowBounds.m_width, windowBounds.m_height);
  if (m_flags & MG_WINDOW_HAS_FRAME)
    m_onFrame->getInsideRect(windowBounds);

  if (m_flags & MG_WINDOW_CAN_MOVE)
    m_pressMode = PRESS_MOVE;
  else m_pressMode = PRESS_NONE;

  if (m_flags & MG_WINDOW_CAN_RESIZE)
  {
    // decide which portion of window frame hit
    if (y < windowBounds.m_y)
    {
      if (x < windowBounds.m_x)
        m_pressMode = PRESS_RESIZE_TL;
      else if (x > windowBounds.right())
        m_pressMode = PRESS_RESIZE_TR;
      else m_pressMode = PRESS_RESIZE_T;
    }
    else if (y > windowBounds.bottom())
    {
      if (x < windowBounds.m_x)
        m_pressMode = PRESS_RESIZE_BL;
      else if (x > windowBounds.right())
        m_pressMode = PRESS_RESIZE_BR;
      else m_pressMode = PRESS_RESIZE_B;
    }
    else
    {
      if (x < windowBounds.m_x)
        m_pressMode = PRESS_RESIZE_L;
      else if (x > windowBounds.right())
        m_pressMode = PRESS_RESIZE_R;
    }
  }

  m_dragX = x;
  m_dragY = y;
}

//--------------------------------------------------------------
// mouse released
void mgSimpleWindow::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  m_pressMode = PRESS_NONE;
}

//--------------------------------------------------------------
// mouse clicked
void mgSimpleWindow::mouseClick(
  void* source,
  int x,
  int y,
  int modifiers,
  int button,
  int clickCount)
{
}

//--------------------------------------------------------------
// mouse dragged
void mgSimpleWindow::mouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
  mgRectangle bounds;
  getBounds(bounds);

  int dx = x - m_dragX;
  int dy = y - m_dragY;
  m_dragX = x;
  m_dragY = y;

  switch (m_pressMode)
  {
    case PRESS_RESIZE_TL: 
    {
      bounds.m_y += dy;
      bounds.m_height -= dy;
      bounds.m_x += dx;
      bounds.m_width -= dx;
      setBounds(bounds);
      break;
    }
    case PRESS_RESIZE_T: 
    {
      bounds.m_y += dy;
      bounds.m_height -= dy;
      setBounds(bounds);
      break;
    }
    case PRESS_RESIZE_TR: 
    {
      bounds.m_y += dy;
      bounds.m_height -= dy;
      bounds.m_width += dx;
      setBounds(bounds);
      break;
    }
    case PRESS_RESIZE_L: 
    {
      bounds.m_x += dx;
      bounds.m_width -= dx;
      setBounds(bounds);
      break;
    }
    case PRESS_RESIZE_R: 
    {
      bounds.m_width += dx;
      setBounds(bounds);
      break;
    }
    case PRESS_RESIZE_BL: 
    {
      bounds.m_height += dy;
      bounds.m_x += dx;
      bounds.m_width -= dx;
      setBounds(bounds);
      break;
    }
    case PRESS_RESIZE_BR: 
    {
      bounds.m_height += dy;
      bounds.m_width += dx;
      setBounds(bounds);
      break;
    }
    case PRESS_RESIZE_B:
    {
      bounds.m_height += dy;
      setBounds(bounds);
      break;
    }
    case PRESS_MOVE:
    {
      bounds.m_x += dx;
      bounds.m_y += dy;
      setLocation(bounds.m_x, bounds.m_y);
      break;
    }
  }

  // update location bounds if setting window loc
  int windowX = bounds.m_x;
  int windowY = bounds.m_y;

  // figure new content bounds
  if (m_flags & MG_WINDOW_HAS_FRAME)
    m_onFrame->getInsideRect(bounds);
  bounds.m_y += m_barHeight;
  bounds.m_height -= m_barHeight;

  m_locationBounds.m_width = bounds.m_width;
  m_locationBounds.m_height = bounds.m_height;

  if (m_locationContent)
  {
    m_locationBounds.m_x = bounds.m_x;
    m_locationBounds.m_y = bounds.m_y;
  }
  else
  {
    m_locationBounds.m_x = windowX;
    m_locationBounds.m_y = windowY;
  }

  // have to switch to TL for any moved windows
  // =-= could figure new position relative to current origin.
  m_locationOrigin = MG_WINDOW_ORIGIN_TL;

  if (m_pressMode == PRESS_MOVE)
    m_desktop->windowMove(m_contentPane);
  else m_desktop->windowResize(m_contentPane);
}

//--------------------------------------------------------------
// mouse moved
void mgSimpleWindow::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

