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
#include "UglyStyle/mgUglyTabs.h"
#include "UglyStyle/mgUglyStyle.h"

class mgUglyTabState
{
public:
  mgControl* m_pane;
  mgString m_label;
  mgString m_name;
  BOOL m_selected;
  int m_x;
  int m_width;
};

//--------------------------------------------------------------
// constructor
mgUglyTabs::mgUglyTabs(
  mgControl* parent,
  const char* cntlName)
: mgControl(parent, cntlName)
{
/*
  mgStyle* style = getStyle();
  m_font = style->getFont(STYLE_TABBEDPANE_FONT);
  style->getColor(STYLE_TABBEDPANE_TEXTCOLOR, m_color);
  style->getColor(STYLE_TABBEDPANE_HOVER_COLOR, m_hoverColor);
  style->getColor(STYLE_TABBEDPANE_DOWN_COLOR, m_downColor);
  style->getColor(STYLE_TABBEDPANE_DIS_COLOR, m_disColor);
  
  m_upFrame = style->getFrame(STYLE_TABBEDPANE_TAB_FRAME);
  m_hoverFrame = style->getFrame(STYLE_TABBEDPANE_HOVER_FRAME);
  m_downFrame = style->getFrame(STYLE_TABBEDPANE_DOWN_FRAME);
  m_disFrame = style->getFrame(STYLE_TABBEDPANE_DIS_FRAME);
  m_childFrame = style->getFrame(STYLE_TABBEDPANE_CHILD_FRAME);
*/
  m_hover = false;
  
  // figure height of tabs
  mgDimension lblSize(0, m_font->getHeight());
  mgDimension extSize;
  if (m_upFrame != NULL)
  {
    m_upFrame->getOutsideSize(lblSize, extSize);
    m_tabHeight = extSize.m_height;
  }
  else m_tabHeight = m_font->getHeight();
  
  addMouseListener((mgMouseListener*) this); 
}

//--------------------------------------------------------------
// destructor
mgUglyTabs::~mgUglyTabs()
{
  // delete all the pane state
  for (int i = 0; i < m_tabs.length(); i++)
  {
    delete (mgUglyTabState*) m_tabs[i];
  }
  
  m_tabs.removeAll();
}

//--------------------------------------------------------------
// add pane to stack
mgControl* mgUglyTabs::addPane(
  const char* label,
  const char* name)
{
  mgUglyTabState* tab = new mgUglyTabState();
  tab->m_pane = new mgControl(this);
  new mgColumnLayout(tab->m_pane);
  
  tab->m_label = label;
  tab->m_name = name;
  tab->m_selected = false;
  tab->m_x = 0;
  tab->m_width = 0;
  
  m_tabs.add(tab);

  mgDimension size;
  getSize(size);
  
  mgRectangle inside(0, m_tabHeight, size.m_width, size.m_height - m_tabHeight);
  if (m_childFrame != NULL)
    m_childFrame->getInsideRect(inside);
    
  tab->m_pane->setVisible(false);
  tab->m_pane->setLocation(inside.m_x, inside.m_y);
  tab->m_pane->setSize(inside.m_width, inside.m_height);
  
  damage();
  
  return tab->m_pane;
}

//--------------------------------------------------------------
// remove a pane from the stack
void mgUglyTabs::removePane(
  const char* name)
{
  // find the named tab
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgUglyTabState* tab = (mgUglyTabState*) m_tabs[i];
    if (tab->m_name.equals(name))
    {
      m_tabs.removeAt(i);
      delete tab;
      break;
    }
  }
  damage();
}
  
//--------------------------------------------------------------
// select pane to show
void mgUglyTabs::selectPane(
  const char* name)
{
  // for all panes
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgUglyTabState* tab = (mgUglyTabState*) m_tabs[i];
    
    // set only selected pane visible
    if (tab->m_name.equals(name))
    {
      tab->m_pane->setVisible(true);
      tab->m_selected = true;
    }
    else
    {
      tab->m_pane->setVisible(false);
      tab->m_selected = false;
    }
  } 
  damage();
}
  
//--------------------------------------------------------------
// get selected pane
void mgUglyTabs::getSelected(
  mgString& name)
{
  name.empty();

  // for all panes
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgUglyTabState* tab = (mgUglyTabState*) m_tabs[i];
    if (tab->m_selected)
    {
      name = tab->m_name;
      break;
    }
  }
}
    
//--------------------------------------------------------------
// get minimum size of control
void mgUglyTabs::minimumSize(
  mgDimension& size)
{
  // return max of min size for all panes.
  size.m_width = 0;
  size.m_height = 0;
  
  // for all panes
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgUglyTabState* tab = (mgUglyTabState*) m_tabs[i];
    
    mgDimension paneSize;
    tab->m_pane->minimumSize(paneSize);
    size.m_width = max(paneSize.m_width, size.m_width);
    size.m_height = max(paneSize.m_height, size.m_height);
  }
  size.m_height += m_tabHeight;
}

//--------------------------------------------------------------
// get preferred size of control
void mgUglyTabs::preferredSize(
  mgDimension& size)
{
  // return max of min size for all panes.
  size.m_width = 0;
  size.m_height = 0;
  
  // for all panes
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgUglyTabState* tab = (mgUglyTabState*) m_tabs[i];
    
    mgDimension paneSize;
    tab->m_pane->preferredSize(paneSize);
    size.m_width = max(paneSize.m_width, size.m_width);
    size.m_height = max(paneSize.m_height, size.m_height);
  }
  size.m_height += m_tabHeight;
}

//--------------------------------------------------------------
// set control size
void mgUglyTabs::setSize(
  int width,
  int height)
{
  mgControl::setSize(width, height);
  
  mgRectangle inside(0, m_tabHeight, width, height - m_tabHeight);
  if (m_childFrame != NULL)
    m_childFrame->getInsideRect(inside);
    
  // all panes superimposed within control
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgUglyTabState* tab = (mgUglyTabState*) m_tabs[i];
    
    tab->m_pane->setLocation(inside.m_x, inside.m_y);
    tab->m_pane->setSize(inside.m_width, inside.m_height);
  }
}

//--------------------------------------------------------------
// mouse entered
void mgUglyTabs::mouseEnter(
  void* source,
  int x,
  int y)
{
  m_hover = true;
  damage();
}

//--------------------------------------------------------------
// mouse exited
void mgUglyTabs::mouseExit(
  void* source)
{
  m_hover = false;
  damage();
}

//--------------------------------------------------------------
// mouse pressed
void mgUglyTabs::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // find selected pane
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgUglyTabState* tab = (mgUglyTabState*) m_tabs[i];
    if (x >= tab->m_x && x < tab->m_x + tab->m_width)
    {
      selectPane(tab->m_name);
      break;
    }
  }
}

//--------------------------------------------------------------
// mouse released
void mgUglyTabs::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
}

//--------------------------------------------------------------
// mouse clicked
void mgUglyTabs::mouseClick(
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
void mgUglyTabs::mouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

//--------------------------------------------------------------
// mouse moved
void mgUglyTabs::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

//--------------------------------------------------------------
// paint content of control
void mgUglyTabs::paint(
  mgContext* gc) 
{
  mgDimension size;
  getSize(size);

  mgRectangle childRect(0, m_tabHeight, size.m_width, size.m_height - m_tabHeight);
  if (m_childFrame != NULL)
    m_childFrame->paintBackground(gc, childRect.m_x, childRect.m_y, childRect.m_width, childRect.m_height);
  
  int x = 0;
  int lblHeight = m_font->getHeight();
  gc->setFont(m_font);
  
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgUglyTabState* tab = (mgUglyTabState*) m_tabs[i];
    
    // figure size of label
    int lblWidth = m_font->stringWidth(tab->m_label, tab->m_label.length());
    mgDimension lblSize(lblWidth, lblHeight);
    mgRectangle lblRect(x, 0, lblWidth, lblHeight);
    
    // choose frame to use
    const mgFrame* lblFrame = NULL;
    mgColor lblColor;
    if (tab->m_selected)
    {
      lblFrame = m_downFrame;
      lblColor = m_downColor;
    }
    else
    {
      lblFrame = m_upFrame;
      lblColor = m_color;
    }
    
    // figure size and draw frame
    if (lblFrame != NULL)
    {
      mgDimension extSize;
      lblFrame->getOutsideSize(lblSize, extSize);
      lblSize = extSize;
      
      lblRect.m_width = lblSize.m_width;
      lblRect.m_height = lblSize.m_height;
      lblFrame->getInsideRect(lblRect);
      
      lblFrame->paintBackground(gc, x, 0, lblSize.m_width, lblSize.m_height);
    }
    
    // draw the label
    gc->setTextColor(lblColor);
    gc->drawString(tab->m_label, tab->m_label.length(), lblRect.m_x, 
                   lblRect.m_y+m_font->getAscent());
                   
    // complete the frame
    if (lblFrame != NULL)
      lblFrame->paintForeground(gc, x, 0, lblSize.m_width, lblSize.m_height);

    // record drawing position for selection test
    tab->m_x = x;
    tab->m_width = lblSize.m_width;
    
    // next tab
    x += lblSize.m_width;
  }

  if (m_childFrame != NULL)
    m_childFrame->paintForeground(gc, childRect.m_x, childRect.m_y, childRect.m_width, childRect.m_height);
}

  