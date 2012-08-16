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
#include "UglyStyle/mgUglySplit.h"
#include "UglyStyle/mgUglyStyle.h"

class mgPaneEntry
{
public:
  mgControl* m_control;
  int m_weight;
  
  mgPaneEntry()
  {
    m_control = NULL;
    m_weight = 0;
  }
};

//--------------------------------------------------------------
// constructor
mgUglySplit::mgUglySplit(
  mgControl* parent,
  const char* cntlName)
: mgControl(parent, cntlName)
{
//  m_vertical = vertical;
/*
  // get divider width
  mgContext* gc = getGraphics();
  mgStyle* style = getStyle();
  m_font = style->getFont(STYLE_SPLITPANE_FONT);
  gc->setFont(m_font);
  
  m_dividerWidth = gc->stringWidth("n", 1);
  delete gc;
  
  m_divider = style->getFrame(STYLE_SPLITPANE_DIVIDER);
  m_vertCursor = style->getCursor(STYLE_SPLITPANE_VERT_CURSOR);
  m_horzCursor = style->getCursor(STYLE_SPLITPANE_HORZ_CURSOR);
*/  
  m_dragIndex = -1;

  addMouseListener(this);
}

//--------------------------------------------------------------
// destructor
mgUglySplit::~mgUglySplit()
{
  for (int i = 0; i < m_panes.length(); i++)
  {
    mgPaneEntry* pane = (mgPaneEntry*) m_panes[i];
    delete pane;
  }
  m_panes.removeAll();
}

//--------------------------------------------------------------
// set vertical or horizontal split
void mgUglySplit::setVertical(
  BOOL vertical)
{
  m_vertical = vertical;

  damage();
}

//--------------------------------------------------------------
// add a new pane to the split
mgControl* mgUglySplit::addPane(
  int weight)
{
  mgPaneEntry* pane = new mgPaneEntry();
  
  // create control and default layout
  pane->m_control = new mgControl(this);
  new mgColumnLayout(pane->m_control);  
  
  pane->m_weight = weight;

  m_panes.add(pane);

  resizePanes();
  
  return pane->m_control;
}

//--------------------------------------------------------------
// resize the panes based on their weights
void mgUglySplit::resizePanes()
{
  mgDimension size;
  getSize(size);

  // get total weight
  int totalWeight = 0;
  for (int i = 0; i < m_panes.length(); i++)
  {
    mgPaneEntry* pane = (mgPaneEntry*) m_panes.getAt(i);
    totalWeight += pane->m_weight;
  }

  // get size after dividers
  int useableSize;
  if (m_vertical)
    useableSize = size.m_width - (m_panes.length()-1)*m_dividerWidth;
  else useableSize = size.m_height - (m_panes.length()-1)*m_dividerWidth;
  useableSize = max(0, useableSize);

  // set pane sizes
  int x = 0;
  int y = 0;
  for (int i = 0; i < m_panes.length(); i++)
  {
    mgPaneEntry* pane = (mgPaneEntry*) m_panes.getAt(i);
    if (m_vertical)
    {
      int width = (useableSize * pane->m_weight) / totalWeight;
      pane->m_control->setLocation(x, 0);
      pane->m_control->setSize(width, size.m_height);
      x += width + m_dividerWidth;
    }
    else
    {
      int height = (useableSize * pane->m_weight) / totalWeight;
      pane->m_control->setLocation(0, y);
      pane->m_control->setSize(size.m_width, height);
      y += height + m_dividerWidth;
    }
  }
}

//--------------------------------------------------------------
// return index of pane divider under point
int mgUglySplit::findDivider(
  int cursorX,
  int cursorY)
{
  int x = 0;
  int y = 0;
  for (int i = 0; i < m_panes.length(); i++)
  {
    mgPaneEntry* pane = (mgPaneEntry*) m_panes.getAt(i);
    mgDimension paneSize;
    pane->m_control->getSize(paneSize);
    if (m_vertical)
    {
      int right = x + paneSize.m_width;
      if (right <= cursorX && cursorX < right+m_dividerWidth)
        return i;
    }
    else
    {
      int bottom = y + paneSize.m_height;
      if (bottom <= cursorY && cursorY < bottom+m_dividerWidth)
        return i;
    }
  }
  return -1;
}

//--------------------------------------------------------------
// paint content of control
void mgUglySplit::paint(
  mgContext* gc) 
{
  if (m_divider == NULL)
    return;  // nothing to do 
    
  mgDimension size;
  getSize(size);
  
  mgRectangle bounds;
  int x = 0;
  int y = 0;
  for (int i = 0; i < m_panes.length(); i++)
  {
    mgPaneEntry* pane = (mgPaneEntry*) m_panes.getAt(i);
    mgDimension paneSize;
    pane->m_control->getSize(paneSize);
    if (m_vertical)
    {
      bounds.m_x = x + paneSize.m_width;
      bounds.m_y = 0;
      bounds.m_width = m_dividerWidth;
      bounds.m_height = size.m_height;
      m_divider->paintBackground(gc, bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height);
      m_divider->paintForeground(gc, bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height);
    }
    else
    {
      bounds.m_x = 0;
      bounds.m_y = y + paneSize.m_height;
      bounds.m_width = size.m_width;
      bounds.m_height = m_dividerWidth;
      m_divider->paintBackground(gc, bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height);
      m_divider->paintForeground(gc, bounds.m_x, bounds.m_y, bounds.m_width, bounds.m_height);
    }
  }
}

//--------------------------------------------------------------
// return minimum size of control
void mgUglySplit::minimumSize(
  mgDimension& size) 
{
  preferredSize(size);
}

//--------------------------------------------------------------
// return preferred size of control
void mgUglySplit::preferredSize(
  mgDimension& size) 
{
  // sum of children preferred sizes
  size.m_width = 0;
  size.m_height = 0;
  for (int i = 0; i < m_panes.length(); i++)
  {
    mgPaneEntry* pane = (mgPaneEntry*) m_panes.getAt(i);
    mgDimension paneSize;
    pane->m_control->preferredSize(paneSize);
    if (m_vertical)
    {
      size.m_height = max(size.m_height, paneSize.m_height);
      size.m_width += paneSize.m_width;
    }
    else
    {
      size.m_width = max(size.m_width, paneSize.m_width);
      size.m_height += paneSize.m_height;
    }
  }
  if (m_vertical)
    size.m_width += (m_panes.length()-1) * m_dividerWidth;
  else size.m_height += (m_panes.length()-1) * m_dividerWidth;
}

//--------------------------------------------------------------
// mouse entered
void mgUglySplit::mouseEnter(
  void* source,
  int x,
  int y)
{
//  setCursor(m_vertical ? m_vertCursor : m_horzCursor);
  mouseMove(source, x, y, 0);
}

//--------------------------------------------------------------
// mouse exited
void mgUglySplit::mouseExit(
  void* source)
{
  // show the normal cursor
//  setCursor(NULL);
}

//--------------------------------------------------------------
// mouse pressed
void mgUglySplit::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // if over a divider, start drag
  m_dragIndex = findDivider(x, y);
  m_dragX = x;
  m_dragY = y;
}

//--------------------------------------------------------------
// mouse released
void mgUglySplit::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // if dragging
  if (m_dragIndex != -1)
  {
    // resize two adjacent panes
    mgPaneEntry* first = (mgPaneEntry*) m_panes.getAt(m_dragIndex);
    mgPaneEntry* second = (mgPaneEntry*) m_panes.getAt(m_dragIndex+1);
    mgRectangle firstBounds;
    first->m_control->getBounds(firstBounds);
    mgRectangle secondBounds;
    second->m_control->getBounds(secondBounds);

    if (m_vertical)
    {
      int delta = x - m_dragX;
      delta = max(delta, m_dividerWidth-firstBounds.m_width);
      delta = min(delta, secondBounds.m_width-m_dividerWidth);

      firstBounds.m_width += delta;
      secondBounds.m_width -= delta;
      secondBounds.m_x += delta;
    }
    else
    {
      int delta = y - m_dragY;
      delta = max(delta, m_dividerWidth-firstBounds.m_height);
      delta = min(delta, secondBounds.m_height-m_dividerWidth);

      firstBounds.m_height += delta;
      secondBounds.m_height -= delta;
      secondBounds.m_y += delta;
    }

    first->m_control->setBounds(firstBounds);
    second->m_control->setBounds(secondBounds);
  }

  // set weights to window sizes
  for (int i = 0; i < m_panes.length(); i++)
  {
    mgPaneEntry* pane = (mgPaneEntry*) m_panes.getAt(i);
    mgDimension paneSize;
    pane->m_control->getSize(paneSize);
    if (m_vertical)
      pane->m_weight = paneSize.m_width;
    else pane->m_weight = paneSize.m_height;
  }
}

//--------------------------------------------------------------
// mouse clicked
void mgUglySplit::mouseClick(
  void* source,
  int x,
  int y,
  int modifiers,
  int button,
  int clickCount)
{
  mouseDown(source, x, y, modifiers, button);
}

//--------------------------------------------------------------
// mouse dragged
void mgUglySplit::mouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

//--------------------------------------------------------------
// mouse moved
void mgUglySplit::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

//--------------------------------------------------------------
// set control size
void mgUglySplit::setSize(
  int width, 
  int height)
{
  mgControl::setSize(width, height);
  
  resizePanes();
}

