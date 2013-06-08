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
#include "SimpleStyle/mgSimpleScrollPane.h"
#include "SimpleStyle/mgSimpleScrollbar.h"
#include "SimpleStyle/mgSimpleStyle.h"

//--------------------------------------------------------------
// constructor
mgSimpleScrollPane::mgSimpleScrollPane(
  mgControl* parent,
  const char* cntlName)
: mgScrollPaneControl(parent, cntlName)
{
  // create the scrollbars, so we can get preferred size correct
  m_vertScrollbar = new mgSimpleScrollbar(this);
  m_vertScrollbar->setHorizontal(false);
  m_vertScrollbar->setVisible(false);
  m_vertScrollbar->addScrollListener(this);

  m_horzScrollbar = NULL;
  m_horzScrollbar = new mgSimpleScrollbar(this);
  m_horzScrollbar->setHorizontal(true);
  m_horzScrollbar->setVisible(false);
  m_horzScrollbar->addScrollListener(this);

  // create the scrolled pane parent
  m_paneParent = new mgControl(this);

  mgStyle* style = getStyle();
  const mgFont* font;
  style->getFontAttr(MG_STYLE_SCROLLPANE, cntlName, "font", font);

  m_lineHeight = font->getHeight();
}

//--------------------------------------------------------------
// destructor
mgSimpleScrollPane::~mgSimpleScrollPane()
{
}

//--------------------------------------------------------------
// get scrolled pane parent
mgControl* mgSimpleScrollPane::getScrollParent()
{
  return m_paneParent;
}

//--------------------------------------------------------------
// set vertical scroller
void mgSimpleScrollPane::setVScrollbar(
  mgScrollbarControl* scroller)
{
  delete m_vertScrollbar;
  m_vertScrollbar = scroller;
  if (m_vertScrollbar != NULL)
    m_vertScrollbar->addScrollListener(this);
}

//--------------------------------------------------------------
// set horizontal scroller
void mgSimpleScrollPane::setHScrollbar(
  mgScrollbarControl* scroller)
{
  delete m_horzScrollbar;
  m_horzScrollbar = scroller;
  if (m_horzScrollbar != NULL)
    m_horzScrollbar->addScrollListener(this);
}

//--------------------------------------------------------------
// update scrollbars
void mgSimpleScrollPane::updateScrollers()
{
  mgDimension viewSize;
  m_paneParent->getSize(viewSize);

  mgDimension paneSize;
  mgPoint paneLoc;
  mgControl* pane = m_paneParent->getChild(0);
  if (pane != NULL)
  {
    pane->getSize(paneSize);
    pane->getLocation(paneLoc);
  }

  // figure new range and visible, inform scrollers
  if (m_vertScrollbar != NULL)
  {
    m_vertScrollbar->setRange(0, paneSize.m_height);
    m_vertScrollbar->setView(-paneLoc.m_y, viewSize.m_height);
    m_vertScrollbar->setVisible(!m_vertScrollbar->isFullView());
  }
  if (m_horzScrollbar != NULL)
  {
    m_horzScrollbar->setRange(0, paneSize.m_width);
    m_horzScrollbar->setView(-paneLoc.m_x, viewSize.m_width);
    m_horzScrollbar->setVisible(!m_horzScrollbar->isFullView());
  }
}

//--------------------------------------------------------------
// update layout of controls
void mgSimpleScrollPane::updateLayout()
{
  // get size of control
  mgDimension size;
  getSize(size);

  // get size of scrolled pane
  mgDimension paneSize;
  mgPoint paneLoc;
  mgControl* pane = m_paneParent->getChild(0);
  if (pane != NULL)
  {
    pane->getSize(paneSize);
    pane->getLocation(paneLoc);
  }

  // get size of scrollbars
  mgDimension vertSize, horzSize;
  if (m_vertScrollbar != NULL)
    m_vertScrollbar->preferredSize(vertSize);

  if (m_horzScrollbar != NULL)
    m_horzScrollbar->preferredSize(horzSize);

  // are we going to need scrollbars after resize?
  int y = -paneLoc.m_y;
  if (y <= 0 && y+size.m_height-horzSize.m_height >= paneSize.m_height)
    vertSize.m_width = 0;  // fully visible, so no scrollbar

  int x = -paneLoc.m_x;
  if (x <= 0 && x+size.m_width-vertSize.m_width >= paneSize.m_width)
    horzSize.m_height = 0;  // fully visible, so no scrollbar

  if (m_vertScrollbar != NULL)
  {
    m_vertScrollbar->setLocation(size.m_width-vertSize.m_width, 0);
    m_vertScrollbar->setSize(vertSize.m_width, size.m_height-horzSize.m_height);
  }
  
  if (m_horzScrollbar != NULL)
  {
    m_horzScrollbar->setLocation(0, size.m_height-horzSize.m_height);
    m_horzScrollbar->setSize(size.m_width-vertSize.m_width, horzSize.m_height);
  }

  m_paneParent->setLocation(0, 0);
  m_paneParent->setSize(size.m_width - vertSize.m_width, size.m_height - horzSize.m_height);

  updateScrollers();
}


//--------------------------------------------------------------
// get minimum size
void mgSimpleScrollPane::minimumSize(
  mgDimension& size) 
{
  // return preferred size of pane
  mgDimension paneSize;
  mgControl* pane = m_paneParent->getChild(0);
  if (pane != NULL)
    pane->minimumSize(paneSize);

  size = paneSize;

  // make sure there's room for scrollbars
  mgDimension vertSize, horzSize;
  if (m_vertScrollbar != NULL)
  {
    m_vertScrollbar->preferredSize(vertSize);
    size.m_width = max(size.m_width, vertSize.m_width*2);
  }

  if (m_horzScrollbar != NULL)
  {
    m_horzScrollbar->preferredSize(horzSize);
    size.m_height = max(size.m_width, horzSize.m_height*2);
  }
}

//--------------------------------------------------------------
// get preferred size
void mgSimpleScrollPane::preferredSize(
  mgDimension& size) 
{
  // return preferred size of pane
  mgDimension paneSize;
  mgControl* pane = m_paneParent->getChild(0);
  if (pane != NULL)
    pane->preferredSize(paneSize);

  size = paneSize;

  // make sure there's room for scrollbars
  mgDimension vertSize, horzSize;
  if (m_vertScrollbar != NULL)
  {
    m_vertScrollbar->preferredSize(vertSize);
    size.m_width = max(size.m_width, vertSize.m_width*2);
  }

  if (m_horzScrollbar != NULL)
  {
    m_horzScrollbar->preferredSize(horzSize);
    size.m_height = max(size.m_width, horzSize.m_height*2);
  }
}

//--------------------------------------------------------------
// line up
void mgSimpleScrollPane::guiScrollLineUp(
  void* source)
{
  mgControl* pane = m_paneParent->getChild(0);
  if (pane == NULL)
    return;  // nothing to do 

  mgRectangle paneBounds;
  pane->getBounds(paneBounds);

  if (source == (mgScroller*) m_vertScrollbar)
  {
    int y = -paneBounds.m_y;

    // up by one line
    y = max(0, y - m_lineHeight);

    pane->setLocation(paneBounds.m_x, -y);
  }

  else if (source == (mgScroller*) m_horzScrollbar)
  {
    int x = -paneBounds.m_x;

    // left by one line
    x = max(0, x - m_lineHeight);

    pane->setLocation(-x, paneBounds.m_y);
  }

  updateScrollers();
  damage();
}

//--------------------------------------------------------------
// line down
void mgSimpleScrollPane::guiScrollLineDown(
  void* source)
{
  mgControl* pane = m_paneParent->getChild(0);
  if (pane == NULL)
    return;  // nothing to do 

  mgRectangle paneBounds;
  pane->getBounds(paneBounds);

  if (source == (mgScroller*) m_vertScrollbar)
  {
    int y = -paneBounds.m_y;

    // down by a line.  max is last line
    y = min(paneBounds.m_height-m_lineHeight, y + m_lineHeight);

    pane->setLocation(paneBounds.m_x, -y);
  }
  else if (source == (mgScroller*) m_horzScrollbar)
  {
    int x = -paneBounds.m_x;

    // down by a line.  max is last line
    x = min(paneBounds.m_width-m_lineHeight, x + m_lineHeight);

    pane->setLocation(-x, paneBounds.m_y);
  }

  updateScrollers();
  damage();
}

//--------------------------------------------------------------
// page up
void mgSimpleScrollPane::guiScrollPageUp(
  void* source)
{
  mgControl* pane = m_paneParent->getChild(0);
  if (pane == NULL)
    return;  // nothing to do 

  mgRectangle paneBounds;
  pane->getBounds(paneBounds);

  mgDimension viewSize;
  m_paneParent->getSize(viewSize);

  if (source == (mgScroller*) m_vertScrollbar)
  {
    int y = -paneBounds.m_y;

    // up by one page
    y = max(0, y - viewSize.m_height);

    pane->setLocation(paneBounds.m_x, -y);
  }
  else if (source == (mgScroller*) m_horzScrollbar)
  {
    int x = -paneBounds.m_x;

    // left by one page
    x = max(0, x - viewSize.m_width);

    pane->setLocation(-x, paneBounds.m_y);
  }

  updateScrollers();
  damage();
}

//--------------------------------------------------------------
// page down
void mgSimpleScrollPane::guiScrollPageDown(
  void* source)
{
  mgControl* pane = m_paneParent->getChild(0);
  if (pane == NULL)
    return;  // nothing to do 

  mgRectangle paneBounds;
  pane->getBounds(paneBounds);

  mgDimension viewSize;
  m_paneParent->getSize(viewSize);

  if (source == (mgScroller*) m_vertScrollbar)
  {
    int y = -paneBounds.m_y;

    // down by a page.  max is last line
    y = min(paneBounds.m_height-m_lineHeight, y + viewSize.m_height);

    pane->setLocation(paneBounds.m_x, -y);
  }
  else if (source == (mgScroller*) m_horzScrollbar)
  {
    int x = -paneBounds.m_x;

    // right by a page.  max is last line
    x = min(paneBounds.m_width-m_lineHeight, x + viewSize.m_width);

    pane->setLocation(-x, paneBounds.m_y);
  }
  updateScrollers();
  damage();
}

//--------------------------------------------------------------
// set position
void mgSimpleScrollPane::guiScrollSetPosition(
  void* source,
  int value)
{
  mgControl* pane = m_paneParent->getChild(0);
  if (pane == NULL)
    return;  // nothing to do 

  mgRectangle paneBounds;
  pane->getBounds(paneBounds);

  mgDimension viewSize;
  m_paneParent->getSize(viewSize);

  // set scroll position
  if (source == (mgScroller*) m_vertScrollbar)
  {
    int y = -paneBounds.m_y;

    // set position, constrained by bounds
    y = min(paneBounds.m_height-m_lineHeight, max(0, value));

    pane->setLocation(paneBounds.m_x, -y);
  }
  else if (source == (mgScroller*) m_horzScrollbar)
  {
    int x = -paneBounds.m_x;

    // set position, constrained by bounds
    x = min(paneBounds.m_width-m_lineHeight, max(0, value));

    pane->setLocation(-x, paneBounds.m_y);
  }
  updateScrollers();
  damage();
}
