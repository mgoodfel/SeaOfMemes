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

#include "GUI/mgToolkit.h"
#include "UglyStyle/mgUglyButton.h"
#include "UglyStyle/mgUglyStyle.h"

const int MG_BUTTON_UP         = 0;
const int MG_BUTTON_DOWN       = 1;
const int MG_BUTTON_HOVER      = 2;

//--------------------------------------------------------------
// constructor
mgUglyButton::mgUglyButton(
  mgControl* parent,
  const char* cntlName)
: mgButtonControl(parent, cntlName)
{
  m_mode = MG_BUTTON_UP;

  mgStyle* style = getStyle();

  // assume style has set all our format elements
  style->getFontAttr(MG_STYLE_BUTTON, m_cntlName, "font", m_font);

  style->getFrameAttr(MG_STYLE_BUTTON, m_cntlName, "upFrame", m_upFrame);
  style->getFrameAttr(MG_STYLE_BUTTON, m_cntlName, "hoverFrame", m_hoverFrame);
  style->getFrameAttr(MG_STYLE_BUTTON, m_cntlName, "downFrame", m_downFrame);
  style->getFrameAttr(MG_STYLE_BUTTON, m_cntlName, "disFrame", m_disFrame);

  style->getColorAttr(MG_STYLE_BUTTON, m_cntlName, "upColor", m_upColor);
  style->getColorAttr(MG_STYLE_BUTTON, m_cntlName, "hoverColor", m_hoverColor);
  style->getColorAttr(MG_STYLE_BUTTON, m_cntlName, "downColor", m_downColor);
  style->getColorAttr(MG_STYLE_BUTTON, m_cntlName, "disColor", m_disColor);

  m_actionListeners = NULL;
  addMouseListener((mgMouseListener*) this);
  addFocusListener((mgFocusListener*) this);
}

//--------------------------------------------------------------
// destructor
mgUglyButton::~mgUglyButton()
{
  delete m_actionListeners;
  m_actionListeners = NULL;
}

//--------------------------------------------------------------
// set button label
void mgUglyButton::setLabel(
  const char* text)
{
  m_label = text;
}

//--------------------------------------------------------------
// set button icon
void mgUglyButton::setIcon(
  const char* fileName)
{
  m_iconName = fileName;
}

//--------------------------------------------------------------
// return minimum size
void mgUglyButton::minimumSize(
  mgDimension& size)
{
  preferredSize(size);
}

//--------------------------------------------------------------
// return preferred size
void mgUglyButton::preferredSize(
  mgDimension& size)
{
  int width = m_font->stringWidth(m_label, m_label.length());

  mgDimension labelSize(width, m_font->getHeight());
  if (m_upFrame != NULL)
    m_upFrame->getOutsideSize(labelSize, size);
  else size = labelSize;
}

//--------------------------------------------------------------
// paint content of control
void mgUglyButton::paint(
  mgContext* gc)
{
  const mgFrame* frame = NULL;
  mgColor textColor;

  if (getEnabled())
  {
    switch (m_mode)
    {
      case MG_BUTTON_UP: 
        frame = m_upFrame;
        textColor = m_upColor;
        break;
      case MG_BUTTON_HOVER: 
        frame = m_hoverFrame;
        textColor = m_hoverColor;
        break;
      case MG_BUTTON_DOWN: 
        frame = m_downFrame;
        textColor = m_downColor;
        break;
    }
  }
  else 
  {
    frame = m_disFrame;
    textColor = m_disColor;
  }

  gc->setDrawMode(MG_DRAW_RGB);
  gc->setFont(m_font);

  mgDimension size;
  getSize(size);
  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (frame != NULL)
  {
    frame->paintBackground(gc, 0, 0, size.m_width, size.m_height);
    frame->getInsideRect(inside);
  }

  mgRectangle bounds;
  mgPoint endPt;
  m_font->stringExtent(m_label, m_label.length(), endPt, bounds);

  gc->setTextColor(textColor);
  gc->drawString(m_label, m_label.length(), 
                 inside.m_x + (inside.m_width - bounds.m_width)/2, 
                 inside.m_y + (inside.m_height - bounds.m_height)/2 + m_font->getAscent());

  if (frame != NULL)
  {
    frame->paintForeground(gc, 0, 0, size.m_width, size.m_height);
  }
}

//--------------------------------------------------------------
// add a action listener
void mgUglyButton::addActionListener(
  mgActionListener* listener)
{
  if (m_actionListeners == NULL)
    m_actionListeners = new mgPtrArray();
  m_actionListeners->add(listener);
}

//--------------------------------------------------------------
// remove a action listener
void mgUglyButton::removeActionListener(
  mgActionListener* listener)
{
  if (m_actionListeners == NULL)
    return;
  m_actionListeners->remove(listener);
}

//--------------------------------------------------------------
// send event to action listeners
void mgUglyButton::sendActionEvent(
  const char* value)
{
  if (m_actionListeners == NULL)
    return;

  for (int i = 0; i < m_actionListeners->length(); i++)
  {
    mgActionListener* listener = (mgActionListener*) m_actionListeners->getAt(i);
    listener->guiAction(this, value);
  }
}

//--------------------------------------------------------------
// mouse entered
void mgUglyButton::mouseEnter(
  void* source,
  int x,
  int y)
{
  // set mode to hover
  m_mode = MG_BUTTON_HOVER;
  damage();
}

//--------------------------------------------------------------
// mouse exited
void mgUglyButton::mouseExit(
  void* source)
{
  // set mode to up
  m_mode = MG_BUTTON_UP;
  damage();
}

//--------------------------------------------------------------
// mouse pressed
void mgUglyButton::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // set mode to down
  m_mode = MG_BUTTON_DOWN;
  damage();
}

//--------------------------------------------------------------
// mouse released
void mgUglyButton::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // set mode to hover
  mgDimension size;
  getSize(size);
  m_mode = MG_BUTTON_HOVER;
  damage();

  // if inside bounds
  if (x >= 0 && x < size.m_width &&
      y >= 0 && y < size.m_height)
  {
    sendActionEvent(m_cntlName);
  }
}

//--------------------------------------------------------------
// mouse clicked
void mgUglyButton::mouseClick(
  void* source,
  int x,
  int y,
  int modifiers,
  int button,
  int clickCount)
{
  mouseDown(source, x, y, modifiers, button);
  mouseUp(source, x, y, modifiers, button);
}

//--------------------------------------------------------------
// mouse dragged
void mgUglyButton::mouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
  mgDimension size;
  getSize(size);

  // if inside bounds
  if (x >= 0 && x < size.m_width &&
      y >= 0 && y < size.m_height)
  {
    // set mode to pressed if not already
    if (m_mode != MG_BUTTON_DOWN)
    {
      m_mode = MG_BUTTON_DOWN;
      damage();
    }
  }
  else
  {
    // set mode to hover, if not already
    if (m_mode != MG_BUTTON_HOVER)
    {
      m_mode = MG_BUTTON_HOVER;
      damage();
    }
  }
}

//--------------------------------------------------------------
// mouse moved
void mgUglyButton::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
  mgDimension size;
  getSize(size);

  // if inside bounds
  if (x >= 0 && x < size.m_width &&
      y >= 0 && y < size.m_height)
  {
    // set mode to hover if not already
    if (m_mode != MG_BUTTON_HOVER)
    {
      m_mode = MG_BUTTON_HOVER;
      damage();
    }
  }
  else
  {
    // set mode to up, if not already
    if (m_mode != MG_BUTTON_UP)
    {
      m_mode = MG_BUTTON_UP;
      damage();
    }
  }
}

//--------------------------------------------------------------
// focus gained
void mgUglyButton::guiFocusGained(
  void* source)
{
  damage();
}

//--------------------------------------------------------------
// focus lost
void mgUglyButton::guiFocusLost(
  void* source)
{
  damage();
}
