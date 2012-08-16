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
#include "GenericStyle/mgGenericButton.h"
#include "GenericStyle/mgGenericStyle.h"
#include "GenericStyle/mgButtonFace.h"

const int MODE_UP         = 0;
const int MODE_DOWN       = 1;
const int MODE_HOVER      = 2;

//--------------------------------------------------------------
// constructor
mgGenericButton::mgGenericButton(
  mgControl* parent)
: mgButtonControl(parent)
{
  // get the style
  mgGenericStyle* style = (mgGenericStyle*) getStyle();
  if (style == NULL)
    throw new mgException("GenericLabel created with style=NULL");
  m_upFace = style->getFace(MG_GENERIC_BUTTON_UP);
  m_downFace = style->getFace(MG_GENERIC_BUTTON_DOWN);
  m_hoverFace = style->getFace(MG_GENERIC_BUTTON_HOVER);
  m_disabledFace = style->getFace(MG_GENERIC_BUTTON_DISABLED);

  m_mode = MODE_UP;

  m_actionListeners = NULL;
  addMouseListener((mgMouseListener*) this);
  addFocusListener((mgFocusListener*) this);
}

//--------------------------------------------------------------
// destructor
mgGenericButton::~mgGenericButton()
{
  delete m_actionListeners;
  m_actionListeners = NULL;
}

//--------------------------------------------------------------
// set button label
void mgGenericButton::setLabel(
  const char* text)
{
  m_label = text;
  m_args.setAt("label", (const char*) m_label);
}

//--------------------------------------------------------------
// set button icon
void mgGenericButton::setIcon(
  const char* fileName)
{
  m_iconName = fileName;
  m_args.setAt("icon", (const char*) m_iconName);
}

//--------------------------------------------------------------
// return minimum size
void mgGenericButton::minimumSize(
  mgDimension& size)
{
  preferredSize(size);
}

//--------------------------------------------------------------
// return preferred size
void mgGenericButton::preferredSize(
  mgDimension& size)
{
  mgContext* gc = newContext();
  m_upFace->preferredSize(gc, m_args, size);
  delete gc;
}

//--------------------------------------------------------------
// paint content of control
void mgGenericButton::paint(
  mgContext* gc)
{
  mgDimension size(m_width, m_height);
  // draw the face with the label string
  m_args.setAt("label", (const char*) m_label);
  m_args.setAt("iconName", (const char*) m_iconName);

  if (getEnabled())
  {
    switch (m_mode)
    {
      case MODE_UP: 
        m_upFace->paint(gc, m_args, size);
        break;
      case MODE_HOVER: 
        m_hoverFace->paint(gc, m_args, size);
        break;
      case MODE_DOWN: 
        m_downFace->paint(gc, m_args, size);
        break;
    }
  }
  else m_disabledFace->paint(gc, m_args, size);
}

//--------------------------------------------------------------
// add a action listener
void mgGenericButton::addActionListener(
  mgActionListener* listener)
{
  if (m_actionListeners == NULL)
    m_actionListeners = new mgPtrArray();
  m_actionListeners->add(listener);
}

//--------------------------------------------------------------
// remove a action listener
void mgGenericButton::removeActionListener(
  mgActionListener* listener)
{
  if (m_actionListeners == NULL)
    return;
  m_actionListeners->remove(listener);
}

//--------------------------------------------------------------
// send event to action listeners
void mgGenericButton::sendActionEvent(
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
void mgGenericButton::mouseEnter(
  void* source,
  int x,
  int y)
{
  // set mode to hover
  m_mode = MODE_HOVER;
  damage();
}

//--------------------------------------------------------------
// mouse exited
void mgGenericButton::mouseExit(
  void* source)
{
  // set mode to up
  m_mode = MODE_UP;
  damage();
}

//--------------------------------------------------------------
// mouse pressed
void mgGenericButton::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  takeKeyFocus();

  // set mode to down
  m_mode = MODE_DOWN;
  damage();
}

//--------------------------------------------------------------
// mouse released
void mgGenericButton::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // set mode to hover
  mgDimension size;
  getSize(size);
  m_mode = MODE_HOVER;
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
void mgGenericButton::mouseClick(
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
void mgGenericButton::mouseDrag(
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
void mgGenericButton::mouseMove(
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
    if (m_mode != MODE_HOVER)
    {
      m_mode = MODE_HOVER;
      damage();
    }
  }
  else
  {
    // set mode to up, if not already
    if (m_mode != MODE_UP)
    {
      m_mode = MODE_UP;
      damage();
    }
  }
}

//--------------------------------------------------------------
// focus gained
void mgGenericButton::guiFocusGained(
  void* source)
{
  damage();
}

//--------------------------------------------------------------
// focus lost
void mgGenericButton::guiFocusLost(
  void* source)
{
  damage();
}
