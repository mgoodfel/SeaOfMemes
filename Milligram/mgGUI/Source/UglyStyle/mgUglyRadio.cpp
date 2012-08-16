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

#include "UIToolkit/mgToolkit.h"

#include "UIEvents/mgMouseEvent.h"
#include "UIEvents/mgSelectEvent.h"

#include "UIControls/mgSolidPaint.h"
#include "UIControls/mgFrame.h"
#include "UIControls/mgRadio.h"

#define MODE_RELEASED  0 
#define MODE_HOVER     1 
#define MODE_PRESSED   2 

//--------------------------------------------------------------
// constructor
mgRadio::mgRadio(
  mgControl* parent)
: mgControl(parent)
{
  m_state = false;
  
  // at create, we are group leader
  m_group.add(this);  
  m_groupLead = this;

  m_mode = MODE_RELEASED;

  mgSurface* surface = getSurface();
  m_font = surface->createFont(L"Georgia-12");
  m_upColor = mgColor(L"white");
  m_downColor = mgColor(L"white");
  m_hoverColor = mgColor(L"white");
  m_disColor = mgColor(L"white");

  m_upFrame = NULL;
  m_downFrame = NULL;
  m_hoverFrame = NULL;
  m_disFrame = NULL;

  m_offIcon = surface->createIcon(L"docs/radioOff.gif;docs/radioOffAlpha.gif");
  m_onIcon = surface->createIcon(L"docs/radioOn.gif;docs/radioOnAlpha.gif");

  m_selectListeners = NULL;

  addMouseListener((mgMouseListener*) this);
}

//--------------------------------------------------------------
// destructor
mgRadio::~mgRadio()
{
  delete m_selectListeners;
  m_selectListeners = NULL;
}

//--------------------------------------------------------------
// add a select listener
void mgRadio::addSelectListener(
  mgSelectListener* listener)
{
  if (m_selectListeners == NULL)
    m_selectListeners = new mgPtrArray();
  m_selectListeners->add(listener);
}

//--------------------------------------------------------------
// remove a select listener
void mgRadio::removeSelectListener(
  mgSelectListener* listener)
{
  if (m_selectListeners == NULL)
    return;
  m_selectListeners->remove(listener);
}

//--------------------------------------------------------------
// send event to select listeners
void mgRadio::sendSelectEvent(
  mgSelectEventType type,
  const WCHAR* value)
{
  if (m_selectListeners == NULL)
    return;
  
  mgSelectEvent ev(this, value, type);
  
  for (int i = 0; i < m_selectListeners->length(); i++)
  {
    mgSelectListener* listener = (mgSelectListener*) m_selectListeners->getAt(i);
    switch (ev.m_type)
    {
      case mgSelectOnEvent: 
        listener->selectionOn(ev);
        break;
        
      case mgSelectOffEvent:
        listener->selectionOff(ev);
        break;
    }
  }
}

//--------------------------------------------------------------
// set text
void mgRadio::setText(
  const WCHAR* text)
{
  m_text = text;
  damage();
}

//--------------------------------------------------------------
// set state
void mgRadio::setState(
  BOOL state)
{
  m_state = state;
  damage();
}

//--------------------------------------------------------------
// get state
BOOL mgRadio::getState()
{
  return m_state;
}

//--------------------------------------------------------------
// set radio button group leader
void mgRadio::setGroupLead(
  mgRadio* lead)
{
  m_groupLead = lead;
  m_groupLead->m_group.add(this);

  // =-= remove from group when control destroyed
  // =-= who owns group object?
}

//--------------------------------------------------------------
// mouse entered
void mgRadio::mouseEnter(
  mgMouseEvent& ev) 
{
  m_mode = MODE_HOVER;
  damage();
}

//--------------------------------------------------------------
// mouse exited
void mgRadio::mouseExit(
  mgMouseEvent& ev) 
{
  m_mode = MODE_RELEASED;
  damage();
}

//--------------------------------------------------------------
// mouse pressed
void mgRadio::mouseDown(
  mgMouseEvent& ev) 
{
  m_mode = MODE_PRESSED;
  damage();
}

//--------------------------------------------------------------
// mouse released
void mgRadio::mouseUp(
  mgMouseEvent& ev) 
{
  // set mode to hover
  mgDimension size;
  getSize(size);
  
  // if inside bounds
  if (ev.m_x >= 0 && ev.m_x < size.m_width &&
      ev.m_y >= 0 && ev.m_y < size.m_height)
  {
    // if off, turn this on, and all other controls in group off
    if (!m_state)
    {
      m_state = true;
      
      mgPtrArray* group = &m_groupLead->m_group;
      for (int i = 0; i < group->length(); i++)
      {
        mgRadio* radio = (mgRadio*) group->getAt(i);
        if (radio != this)
        {
          radio->setState(false);
          radio->sendSelectEvent(mgSelectOffEvent, m_cntlName);
        }
      }
      // call listeners with selection
      sendSelectEvent(mgSelectOnEvent, m_cntlName);
    }
  }
  m_mode = MODE_HOVER;
  damage();
}

//--------------------------------------------------------------
// mouse clicked
void mgRadio::mouseClick(
  mgMouseEvent& ev) 
{
  mouseDown(ev);
}

//--------------------------------------------------------------
// mouse dragged
void mgRadio::mouseDrag(
  mgMouseEvent& ev) 
{
  mgDimension size;
  getSize(size);

  // if inside bounds
  if (ev.m_x >= 0 && ev.m_x < size.m_width &&
      ev.m_y >= 0 && ev.m_y < size.m_height)
  {
    // set mode to pressed if not already
    if (m_mode != MODE_PRESSED)
    {
      m_mode = MODE_PRESSED;
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
void mgRadio::mouseMove(
  mgMouseEvent& ev) 
{
}

//--------------------------------------------------------------
// paint content of control
void mgRadio::paint(
  mgContext* gc) 
{
  mgDimension size;
  getSize(size);

  const mgFrame* frame = NULL;
  mgColor lblColor;// = mgBlackColor;

  // pick color and frame to paint
  if (!getEnabled())
  {
    frame = m_disFrame;
    lblColor = m_disColor;
  }
  else
  {
    switch (m_mode)
    {
      case MODE_RELEASED:
        frame = m_upFrame;
        lblColor = m_upColor;
        break;

      case MODE_HOVER:
        frame = m_hoverFrame;
        lblColor = m_hoverColor;
        break;

      case MODE_PRESSED:
        frame = m_downFrame;
        lblColor = m_downColor;
        break;
    }
  }
  
  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (frame != NULL)
  {
    frame->getInsideRect(inside);
    frame->paintBackground(this, gc, 0, 0, size.m_width, size.m_height);
  }
    
  gc->setFont(m_font);

  int blank = gc->stringWidth(L" ", 1);
  // set icon height to line height
  int iconHeight = m_font->getHeight();
  int height = iconHeight;
  int width = iconHeight + blank + gc->stringWidth(m_text, m_text.length());

  int x = (inside.m_width - width)/2;
  int y = (inside.m_height - height)/2;

  // draw the radio button and label
  int chkY = (inside.m_height-iconHeight)/2;

  if (m_state)
    gc->drawIcon(m_onIcon, x+inside.m_x, chkY+inside.m_y, iconHeight, iconHeight);
  else gc->drawIcon(m_offIcon, x+inside.m_x, chkY+inside.m_y, iconHeight, iconHeight);

  x += iconHeight + blank;

  gc->setTextColor(lblColor);
  gc->drawString(m_text, m_text.length(), inside.m_x+x, inside.m_y+y+m_font->getAscent());
}

//--------------------------------------------------------------
// get minimum size
void mgRadio::minimumSize(
  mgDimension& size) 
{
  preferredSize(size);
}

//--------------------------------------------------------------
// get preferred size
void mgRadio::preferredSize(
  mgDimension& size) 
{
  mgContext* gc = getContext();
  
  gc->setFont(m_font);
  
  // set icon height to line height
  int iconHeight = m_font->getHeight();
  size.m_height = iconHeight;
  size.m_width = iconHeight + gc->stringWidth(L" ", 1) + gc->stringWidth(m_text, m_text.length());

  delete gc;

  // add up frame
  if (m_upFrame != NULL)
  {
    mgDimension extSize;
    m_upFrame->getOutsideSize(size, extSize);
    size = extSize;
  }
}
