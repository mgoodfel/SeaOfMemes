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
#include "SimpleStyle/mgSimpleStack.h"
#include "SimpleStyle/mgSimpleStyle.h"

//--------------------------------------------------------------
// constructor
mgSimpleStack::mgSimpleStack(
  mgControl* parent,
  const char* cntlName)
: mgStackControl(parent, cntlName)
{
  m_sameSize = true;
}

//--------------------------------------------------------------
// destructor
mgSimpleStack::~mgSimpleStack()
{
  m_panes.removeAll();
}

//--------------------------------------------------------------
// force all panes to same size
void mgSimpleStack::setSameSize(
  BOOL enable)
{
  m_sameSize = enable;
}
      
//--------------------------------------------------------------
// add pane to stack
mgControl* mgSimpleStack::addPane(
  const char* name)
{
  // create pane and default layout
  mgControl* pane = new mgControl(this, "");
  if (m_sameSize)
    new mgColumnLayout(pane);
  
  // save in table
  m_panes.setAt(name, pane);
  
  mgDimension size;
  getSize(size);

  // position pane
  pane->setVisible(false);
  pane->setLocation(0, 0);
  pane->setSize(size);
  
  return pane;
}

//--------------------------------------------------------------
// remove a pane from the stack
void mgSimpleStack::removePane(
  const char* name)
{
  m_panes.removeKey(name);
}
  
//--------------------------------------------------------------
// select pane to show
void mgSimpleStack::selectPane(
  const char* name)
{
  // find the named pane
  const void* value;
  if (!m_panes.lookup(name, value))
    return;
    
  mgControl* selected = (mgControl*) value;
  selected->setVisible(true);
  
  // turn off the other panes
  int posn = m_panes.getStartPosition();
  while (posn != -1)
  {
    mgString key;
    const void* value;
    m_panes.getNextAssoc(posn, key, value);
    mgControl* pane = (mgControl*) value;
    if (pane != selected)
      pane->setVisible(false);
  }

  m_selected = name;
}
  
//--------------------------------------------------------------
// return selected pane
void mgSimpleStack::getSelected(
  mgString& name)
{
  name = m_selected;
}

//--------------------------------------------------------------
// get minimum size of control
void mgSimpleStack::minimumSize(
  mgDimension& size)
{
  // return max of min size for all panes.
  size.m_width = 0;
  size.m_height = 0;
  
  // turn off the other panes
  int posn = m_panes.getStartPosition();
  while (posn != -1)
  {
    mgString key;
    const void* value;
    m_panes.getNextAssoc(posn, key, value);
    mgControl* pane = (mgControl*) value;
    
    mgDimension paneSize;
    pane->minimumSize(paneSize);
    size.m_width = max(paneSize.m_width, size.m_width);
    size.m_height = max(paneSize.m_height, size.m_height);
  }
}

//--------------------------------------------------------------
// get preferred size of control
void mgSimpleStack::preferredSize(
  mgDimension& size)
{
  // return max of min size for all panes.
  size.m_width = 0;
  size.m_height = 0;
  
  // turn off the other panes
  int posn = m_panes.getStartPosition();
  while (posn != -1)
  {
    mgString key;
    const void* value;
    m_panes.getNextAssoc(posn, key, value);
    mgControl* pane = (mgControl*) value;
    
    mgDimension paneSize;
    pane->preferredSize(paneSize);
    size.m_width = max(paneSize.m_width, size.m_width);
    size.m_height = max(paneSize.m_height, size.m_height);
  }
}

//--------------------------------------------------------------
// set control size
void mgSimpleStack::updateLayout()
{
  mgDimension size;
  getSize(size);

  // all panes superimposed within control
  int posn = m_panes.getStartPosition();
  while (posn != -1)
  {
    mgString key;
    const void* value;
    m_panes.getNextAssoc(posn, key, value);
    mgControl* pane = (mgControl*) value;
    pane->setLocation(0, 0);
    pane->setSize(size.m_width, size.m_height);
  }
}

  