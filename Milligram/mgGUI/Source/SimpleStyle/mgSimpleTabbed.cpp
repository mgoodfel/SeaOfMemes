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
#include "SimpleStyle/mgSimpleTabbed.h"
#include "SimpleStyle/mgSimpleStyle.h"

class mgSimpleTabState
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
mgSimpleTabbed::mgSimpleTabbed(
  mgControl* parent,
  const char* cntlName)
: mgTabbedControl(parent, cntlName)
{
  mgStyle* style = getStyle();
  style->getFontAttr(MG_STYLE_TABBED, cntlName, "font", m_font);

  style->getColorAttr(MG_STYLE_TABBED, cntlName, "upColor", m_upColor);
  style->getFrameAttr(MG_STYLE_TABBED, cntlName, "upFrame", m_upFrame);

  style->getColorAttr(MG_STYLE_TABBED, cntlName, "downColor", m_downColor);
  style->getFrameAttr(MG_STYLE_TABBED, cntlName, "downFrame", m_downFrame);

  style->getColorAttr(MG_STYLE_TABBED, cntlName, "hoverColor", m_hoverColor);
  style->getFrameAttr(MG_STYLE_TABBED, cntlName, "hoverFrame", m_hoverFrame);

  style->getColorAttr(MG_STYLE_TABBED, cntlName, "disColor", m_disColor);
  style->getFrameAttr(MG_STYLE_TABBED, cntlName, "disFrame", m_disFrame);

  style->getFrameAttr(MG_STYLE_TABBED, cntlName, "childFrame", m_childFrame);

  // figure height of tabs
  mgDimension lblSize(0, m_font->getHeight());
  mgDimension extSize;
  if (m_upFrame != NULL)
  {
    m_upFrame->getOutsideSize(lblSize, extSize);
    m_tabHeight = extSize.m_height;
  }
  else m_tabHeight = m_font->getHeight();
  
  m_hover = false;

  addMouseListener((mgMouseListener*) this); 
}

//--------------------------------------------------------------
// destructor
mgSimpleTabbed::~mgSimpleTabbed()
{
  // delete all the pane state
  for (int i = 0; i < m_tabs.length(); i++)
  {
    delete (mgSimpleTabState*) m_tabs[i];
  }
  
  m_tabs.removeAll();
}

//--------------------------------------------------------------
// add pane to stack
mgControl* mgSimpleTabbed::addPane(
  const char* label,
  const char* name)
{
  mgSimpleTabState* tab = new mgSimpleTabState();
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
void mgSimpleTabbed::removePane(
  const char* name)
{
  // find the named tab
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgSimpleTabState* tab = (mgSimpleTabState*) m_tabs[i];
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
void mgSimpleTabbed::selectPane(
  const char* name)
{
  // for all panes
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgSimpleTabState* tab = (mgSimpleTabState*) m_tabs[i];
    
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
void mgSimpleTabbed::getSelected(
  mgString& name)
{
  name.empty();

  // for all panes
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgSimpleTabState* tab = (mgSimpleTabState*) m_tabs[i];
    if (tab->m_selected)
    {
      name = tab->m_name;
      break;
    }
  }
}
    
//--------------------------------------------------------------
// get minimum size of control
void mgSimpleTabbed::minimumSize(
  mgDimension& size)
{
  // return max of min size for all panes.
  size.m_width = 0;
  size.m_height = 0;
  
  // for all panes
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgSimpleTabState* tab = (mgSimpleTabState*) m_tabs[i];
    
    mgDimension paneSize;
    tab->m_pane->minimumSize(paneSize);
    size.m_width = max(paneSize.m_width, size.m_width);
    size.m_height = max(paneSize.m_height, size.m_height);
  }
  size.m_height += m_tabHeight;
}

//--------------------------------------------------------------
// get preferred size of control
void mgSimpleTabbed::preferredSize(
  mgDimension& size)
{
  // return max of preferred size for all panes.
  size.m_width = 0;
  size.m_height = 0;
  
  // for all panes
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgSimpleTabState* tab = (mgSimpleTabState*) m_tabs[i];
    
    mgDimension paneSize;
    tab->m_pane->preferredSize(paneSize);
    size.m_width = max(paneSize.m_width, size.m_width);
    size.m_height = max(paneSize.m_height, size.m_height);
  }
  size.m_height += m_tabHeight;
}

//--------------------------------------------------------------
// set control size
void mgSimpleTabbed::updateLayout()
{
  mgDimension size;
  getSize(size);

  mgRectangle inside(0, m_tabHeight, size.m_width, size.m_height - m_tabHeight);
  if (m_childFrame != NULL)
    m_childFrame->getInsideRect(inside);
    
  // all panes superimposed within control
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgSimpleTabState* tab = (mgSimpleTabState*) m_tabs[i];
    
    tab->m_pane->setLocation(inside.m_x, inside.m_y);
    tab->m_pane->setSize(inside.m_width, inside.m_height);
  }
}

//--------------------------------------------------------------
// mouse entered
void mgSimpleTabbed::mouseEnter(
  void* source,
  int x,
  int y)
{
  m_hover = true;
  damage();
}

//--------------------------------------------------------------
// mouse exited
void mgSimpleTabbed::mouseExit(
  void* source)
{
  m_hover = false;
  damage();
}

//--------------------------------------------------------------
// mouse pressed
void mgSimpleTabbed::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // find selected pane
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgSimpleTabState* tab = (mgSimpleTabState*) m_tabs[i];
    if (x >= tab->m_x && x < tab->m_x + tab->m_width)
    {
      selectPane(tab->m_name);
      break;
    }
  }
}

//--------------------------------------------------------------
// mouse released
void mgSimpleTabbed::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
}

//--------------------------------------------------------------
// mouse clicked
void mgSimpleTabbed::mouseClick(
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
void mgSimpleTabbed::mouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

//--------------------------------------------------------------
// mouse moved
void mgSimpleTabbed::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

//--------------------------------------------------------------
// paint content of control
void mgSimpleTabbed::paint(
  mgContext* gc) 
{
  mgDimension size;
  getSize(size);

  mgRectangle childRect(0, m_tabHeight, size.m_width, size.m_height - m_tabHeight);
  if (m_childFrame != NULL)
  {
    gc->setAlphaMode(MG_ALPHA_SET);
    m_childFrame->paintBackground(gc, childRect.m_x, childRect.m_y, childRect.m_width, childRect.m_height);
  }

  int x = 0;
  int lblHeight = m_font->getHeight();
  gc->setFont(m_font);
  
  for (int i = 0; i < m_tabs.length(); i++)
  {
    mgSimpleTabState* tab = (mgSimpleTabState*) m_tabs[i];
    
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
      lblColor = m_upColor;
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
      
      gc->setAlphaMode(MG_ALPHA_SET);
      lblFrame->paintBackground(gc, x, 0, lblSize.m_width, lblSize.m_height);
    }
    
    // draw the label
    gc->setAlphaMode(MG_ALPHA_MERGE);
    gc->setTextColor(lblColor);
    gc->drawString(tab->m_label, tab->m_label.length(), lblRect.m_x, 
                   lblRect.m_y+m_font->getAscent());
                   
    // complete the frame
    if (lblFrame != NULL)
    {
      gc->setAlphaMode(MG_ALPHA_SET);
      lblFrame->paintForeground(gc, x, 0, lblSize.m_width, lblSize.m_height);
    }

    // record drawing position for selection test
    tab->m_x = x;
    tab->m_width = lblSize.m_width;
    
    // next tab
    x += lblSize.m_width;
  }

  if (m_childFrame != NULL)
  {
    gc->setAlphaMode(MG_ALPHA_SET);
    m_childFrame->paintForeground(gc, childRect.m_x, childRect.m_y, childRect.m_width, childRect.m_height);
  }
}

  