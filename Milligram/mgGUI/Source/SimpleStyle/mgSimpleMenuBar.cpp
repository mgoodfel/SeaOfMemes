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

// A simple menu bar control

#include "GUI/mgToolkit.h"
#include "SimpleStyle/mgSimpleMenuBar.h"

//--------------------------------------------------------------
// constructor
mgSimpleMenuBar::mgSimpleMenuBar(
  mgControl* parent)
: mgMenuBarControl(parent)
{
}

//--------------------------------------------------------------
// destructor
mgSimpleMenuBar::~mgSimpleMenuBar()
{
}
  
//--------------------------------------------------------------
// set menu
void mgSimpleMenuBar::setMenu(
  const mgMenu* menu)
{
}
  
//--------------------------------------------------------------
// add a action listener
void mgSimpleMenuBar::addActionListener(
  mgActionListener* listener)
{
  if (m_actionListeners == NULL)
    m_actionListeners = new mgPtrArray();
  m_actionListeners->add(listener);
}

//--------------------------------------------------------------
// remove a action listener
void mgSimpleMenuBar::removeActionListener(
  mgActionListener* listener)
{
  if (m_actionListeners == NULL)
    return;
  m_actionListeners->remove(listener);
}

//--------------------------------------------------------------
// send event to action listeners
void mgSimpleMenuBar::sendActionEvent(
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
// add a select listener
void mgSimpleMenuBar::addSelectListener(
  mgSelectListener* listener)
{
  if (m_selectListeners == NULL)
    m_selectListeners = new mgPtrArray();
  m_selectListeners->add(listener);
}

//--------------------------------------------------------------
// remove a select listener
void mgSimpleMenuBar::removeSelectListener(
  mgSelectListener* listener)
{
  if (m_selectListeners == NULL)
    return;
  m_selectListeners->remove(listener);
}

//--------------------------------------------------------------
// send event to select listeners
void mgSimpleMenuBar::sendSelectEvent()
{
  if (m_selectListeners == NULL)
    return;
  
  for (int i = 0; i < m_selectListeners->length(); i++)
  {
    mgSelectListener* listener = (mgSelectListener*) m_selectListeners->getAt(i);
//    listener->guiSelection(this, m_cntlName, m_state);
  }
}

//--------------------------------------------------------------
// return minimum size
void mgSimpleMenuBar::minimumSize(
  mgDimension& size)
{
}

//--------------------------------------------------------------
// return preferred size
void mgSimpleMenuBar::preferredSize(
  mgDimension& size)
{
}
    
//--------------------------------------------------------------
// mouse entered
void mgSimpleMenuBar::mouseEnter(
  void* source,
  int x,
  int y)
{
}
    
//--------------------------------------------------------------
// mouse exited
void mgSimpleMenuBar::mouseExit(
  void* source)
{
}
    
//--------------------------------------------------------------
// mouse pressed
void mgSimpleMenuBar::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
}
    
//--------------------------------------------------------------
// mouse released
void mgSimpleMenuBar::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
}
    
//--------------------------------------------------------------
// mouse clicked
void mgSimpleMenuBar::mouseClick(
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
void mgSimpleMenuBar::mouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
}
    
//--------------------------------------------------------------
// mouse moved
void mgSimpleMenuBar::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

