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
#include "SimpleStyle/mgSimpleCheckbox.h"
#include "SimpleStyle/mgSimpleStyle.h"

#define MODE_UP     0 
#define MODE_HOVER  1 
#define MODE_DOWN   2 

//--------------------------------------------------------------
// constructor
mgSimpleCheckbox::mgSimpleCheckbox(
  mgControl* parent,
  const char* cntlName)
: mgCheckboxControl(parent, cntlName)
{
  m_state = false;
  m_mode = MODE_UP;

  mgStyle* style = getStyle();

  // assume style has set all our format elements
  style->getFontAttr(MG_STYLE_CHECKBOX, getName(), "font", m_font);

  style->getFrameAttr(MG_STYLE_CHECKBOX, getName(), "upFrame", m_upFrame);
  style->getFrameAttr(MG_STYLE_CHECKBOX, getName(), "hoverFrame", m_hoverFrame);
  style->getFrameAttr(MG_STYLE_CHECKBOX, getName(), "downFrame", m_downFrame);
  style->getFrameAttr(MG_STYLE_CHECKBOX, getName(), "disFrame", m_disFrame);

  style->getColorAttr(MG_STYLE_CHECKBOX, getName(), "upColor", m_upColor);
  style->getColorAttr(MG_STYLE_CHECKBOX, getName(), "hoverColor", m_hoverColor);
  style->getColorAttr(MG_STYLE_CHECKBOX, getName(), "downColor", m_downColor);
  style->getColorAttr(MG_STYLE_CHECKBOX, getName(), "disColor", m_disColor);

  m_selectListeners = NULL;

  addMouseListener((mgMouseListener*) this);
}

//--------------------------------------------------------------
// destructor
mgSimpleCheckbox::~mgSimpleCheckbox()
{
  delete m_selectListeners;
  m_selectListeners = NULL;
}

//--------------------------------------------------------------
// add a select listener
void mgSimpleCheckbox::addSelectListener(
  mgSelectListener* listener)
{
  if (m_selectListeners == NULL)
    m_selectListeners = new mgPtrArray();
  m_selectListeners->add(listener);
}

//--------------------------------------------------------------
// remove a select listener
void mgSimpleCheckbox::removeSelectListener(
  mgSelectListener* listener)
{
  if (m_selectListeners == NULL)
    return;
  m_selectListeners->remove(listener);
}

//--------------------------------------------------------------
// send event to select listeners
void mgSimpleCheckbox::sendSelectEvent()
{
  if (m_selectListeners == NULL)
    return;
  
  for (int i = 0; i < m_selectListeners->length(); i++)
  {
    mgSelectListener* listener = (mgSelectListener*) m_selectListeners->getAt(i);
    listener->guiSelection(this, getName(), m_state);
  }
}


//--------------------------------------------------------------
// set text
void mgSimpleCheckbox::setLabel(
  const char* label)
{
  m_label = label;
  damage();
}

//--------------------------------------------------------------
// set state
void mgSimpleCheckbox::setState(
  BOOL state)
{
  m_state = state;
  damage();
}

//--------------------------------------------------------------
// get state
BOOL mgSimpleCheckbox::getState()
{
  return m_state;
}

//--------------------------------------------------------------
// mouse entered
void mgSimpleCheckbox::mouseEnter(
  void* source,
  int x,
  int y)
{
  m_mode = MODE_HOVER;
  damage();
}

//--------------------------------------------------------------
// mouse exited
void mgSimpleCheckbox::mouseExit(
  void* source)
{
  m_mode = MODE_UP;
  damage();
}

//--------------------------------------------------------------
// mouse pressed
void mgSimpleCheckbox::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  m_mode = MODE_DOWN;
  damage();
}

//--------------------------------------------------------------
// mouse released
void mgSimpleCheckbox::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // set mode to hover
  mgDimension size;
  getSize(size);

  // if inside bounds
  if (x >= 0 && x < size.m_width &&
      y >= 0 && y < size.m_height)
  {
    // invert flag
    m_state = !m_state;

    // call listeners with selection
    sendSelectEvent();
  }
  m_mode = MODE_HOVER;
  damage();
}

//--------------------------------------------------------------
// mouse clicked
void mgSimpleCheckbox::mouseClick(
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
void mgSimpleCheckbox::mouseDrag(
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
    if (m_mode != MODE_DOWN)
    {
      m_mode = MODE_DOWN;
      damage();
    }
  }
  else
  {
    // set mode to hover, if not already
    if (m_mode != MODE_HOVER)
    {
      m_mode = MODE_HOVER;
      damage();
    }
  }
}

//--------------------------------------------------------------
// mouse moved
void mgSimpleCheckbox::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

//--------------------------------------------------------------
// paint content of control
void mgSimpleCheckbox::paint(
  mgContext* gc) 
{
  gc->setFont(m_font);

  const mgFrame* frame = NULL;
  mgColor textColor;

  if (getEnabled())
  {
    switch (m_mode)
    {
      case MODE_UP: 
        frame = m_upFrame;
        break;
      case MODE_HOVER: 
        frame = m_hoverFrame;
        break;
      case MODE_DOWN: 
        frame = m_downFrame;
        break;
    }
  }
  else 
  {
    frame = m_disFrame;
  }

  gc->setAlphaMode(MG_ALPHA_MERGE);
  gc->setTextColor(textColor);

  mgDimension size;
  getSize(size);

  int iconHeight = m_font->getHeight();
  int y = (size.m_height - iconHeight)/2;

  if (frame != NULL)
  {
    frame->paintBackground(gc, 0, y, iconHeight, iconHeight);
    if (m_state)
    {
      int xwidth = m_font->stringWidth("x", 1);
      gc->drawString("x", 1, (iconHeight-xwidth)/2, y+m_font->getAscent());
    }
    frame->paintForeground(gc, 0, y, iconHeight, iconHeight);
  }
    
  int blank = m_font->stringWidth(" ", 1);
  int x = iconHeight + blank;

  gc->setTextColor(textColor);
  gc->drawString(m_label, m_label.length(), x, y+m_font->getAscent());
}

//--------------------------------------------------------------
// get minimum size
void mgSimpleCheckbox::minimumSize(
  mgDimension& size) 
{
  preferredSize(size);
}

//--------------------------------------------------------------
// get preferred size
void mgSimpleCheckbox::preferredSize(
  mgDimension& size) 
{
  // scale icon to height of line
  int iconHeight = m_font->getHeight();
  size.m_height = iconHeight;
  size.m_width = iconHeight + m_font->stringWidth(" ", 1) + m_font->stringWidth(m_label, m_label.length());
}
  
