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
#include "SimpleStyle/mgSimpleList.h"
#include "SimpleStyle/mgSimpleScrollbar.h"
#include "SimpleStyle/mgSimpleStyle.h"

// an entry in the list
class mgListEntry
{
public:
  BOOL m_selected;
  mgString m_text;
  const mgIcon* m_icon;
  mgString m_name;
};

//--------------------------------------------------------------
// constructor
mgSimpleList::mgSimpleList(
  mgControl* parent,
  const char* cntlName)
: mgListControl(parent, cntlName)
{
  m_selected = -1;
  m_displayLines = 5;
  m_multiSelect = false;
  m_hasIcons = false;
  m_scrollPosn = 0;
  m_hover = -1;
  
  mgStyle* style = getStyle();
  style->getFontAttr(MG_STYLE_LIST, getName(), "font", m_font);
  
  style->getFrameAttr(MG_STYLE_LIST, getName(), "upFrame", m_upFrame);
  style->getFrameAttr(MG_STYLE_LIST, getName(), "disFrame", m_disFrame);

  style->getFrameAttr(MG_STYLE_LIST, getName(), "itemUpFrame", m_itemUpFrame);
  style->getFrameAttr(MG_STYLE_LIST, getName(), "itemHoverFrame", m_itemHoverFrame);
  style->getFrameAttr(MG_STYLE_LIST, getName(), "itemDownFrame", m_itemDownFrame);
  style->getFrameAttr(MG_STYLE_LIST, getName(), "itemDisFrame", m_itemDisFrame);

  style->getColorAttr(MG_STYLE_LIST, getName(), "itemUpColor", m_itemUpColor);
  style->getColorAttr(MG_STYLE_LIST, getName(), "itemHoverColor", m_itemHoverColor);
  style->getColorAttr(MG_STYLE_LIST, getName(), "itemDownColor", m_itemDownColor);
  style->getColorAttr(MG_STYLE_LIST, getName(), "itemDisColor", m_itemDisColor);

  // set icon height to 80% line height
  m_iconHeight = (m_font->getHeight()*8)/10;
  m_lineHeight = max(m_iconHeight, m_font->getHeight());

  // add any item framing -- all frames should be same size as itemUpFrame
  if (m_itemUpFrame != NULL)
  {
    mgDimension size;
    m_itemUpFrame->getOutsideSize(mgDimension(0, m_lineHeight), size);
    m_lineHeight = size.m_height;
  }
      
  // create the scrollbars, so we can get preferred size correct
  m_vertScroller = new mgSimpleScrollbar(this);
  m_vertScroller->setHorizontal(false);
  m_vertScroller->setVisible(false);
  m_vertScroller->addScrollListener(this);

  m_horzScroller = NULL;
/* =-= not used or laid out correctly
  m_horzScroller = new mgSimpleScrollbar(this);
  m_horzScroller->setHorizontal(true);
  m_horzScroller->setVisible(false);
  m_horzScroller->addScrollListener(this);
*/

  m_selectListeners = NULL;

  addMouseListener(this);
  addFocusListener(this);
}

//--------------------------------------------------------------
// destructor
mgSimpleList::~mgSimpleList()
{
  for (int i = 0; i < m_entries.length(); i++)
  {
    mgListEntry* entry = (mgListEntry*) m_entries.getAt(i);
    delete entry;
  }
  m_entries.removeAll();

  delete m_selectListeners;
  m_selectListeners = NULL;
}

//--------------------------------------------------------------
// add a select listener
void mgSimpleList::addSelectListener(
  mgSelectListener* listener)
{
  if (m_selectListeners == NULL)
    m_selectListeners = new mgPtrArray();
  m_selectListeners->add(listener);
}

//--------------------------------------------------------------
// remove a select listener
void mgSimpleList::removeSelectListener(
  mgSelectListener* listener)
{
  if (m_selectListeners == NULL)
    return;
  m_selectListeners->remove(listener);
}

#ifdef WORKED
//--------------------------------------------------------------
// send event to select listeners
void mgSimpleList::sendSelectEvent(
  mgSelectEventType type,
  const char* value)
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
#endif

//--------------------------------------------------------------
// add an entry with text and icon
void mgSimpleList::addEntry(
  const char* name,
  const char* text,
  const mgIcon* icon)
{
  mgListEntry* entry = new mgListEntry();
  entry->m_selected = false;
  entry->m_name = name;
  entry->m_icon = icon;
  entry->m_text = (text != NULL && strlen(text) != 0) ? text : name;
  m_entries.add(entry);

  if (entry->m_icon != NULL)
    m_hasIcons = true;

  damage();
}

//--------------------------------------------------------------
// set multiple select mode
void mgSimpleList::setMultiSelect(
  BOOL multiSelect)
{
  m_multiSelect = multiSelect;
  
  // if not mutliselect, turn off all but first selection
  if (!m_multiSelect)
  {
    for (int i = 0; i < m_entries.length(); i++)
    {
      mgListEntry* entry = (mgListEntry*) m_entries.getAt(i);
      if (entry->m_selected)
      {
        if (m_selected != -1)
          m_selected = i;
        entry->m_selected = false;
      }
    }
  }
}

//--------------------------------------------------------------
// set item selected
void mgSimpleList::setItemState(
  const char* name,
  BOOL selected)
{
  for (int i = 0; i < m_entries.length(); i++)
  {
    mgListEntry* entry = (mgListEntry*) m_entries.getAt(i);
    if (entry->m_name.equals(name))
    {
      if (m_multiSelect)
        entry->m_selected = selected;
      else m_selected = i;
      break;
    }
  }
}

//--------------------------------------------------------------
// return true if item selected
BOOL mgSimpleList::getItemState(
  const char* name)
{
  for (int i = 0; i < m_entries.length(); i++)
  {
    mgListEntry* entry = (mgListEntry*) m_entries.getAt(i);
    if (entry->m_name.equals(name))
      return entry->m_selected;
  }
  // couldn't find item, so it's not selected =-= throw exception?
  return false; 
}

//--------------------------------------------------------------
// get count of selected items
int mgSimpleList::getSelectionCount()
{
  if (m_multiSelect)
  {
    int count = 0;
    for (int i = 0; i < m_entries.length(); i++)
    {
      mgListEntry* entry = (mgListEntry*) m_entries.getAt(i);
      if (entry->m_selected)
        count++;
    }
    return count;
  }
  else
  {
    return m_selected != -1 ? 1 : 0;
  }
}
  
//--------------------------------------------------------------
// get nth selected item
void mgSimpleList::getSelectedItem(
  int posn,
  mgString &name)
{
  int count = 0;
  for (int i = 0; i < m_entries.length(); i++)
  {
    mgListEntry* entry = (mgListEntry*) m_entries.getAt(i);
    if (entry->m_selected)
    {
      if (posn == count)
      {
        name = entry->m_name;
        return;
      }
      count++;
    }
  }
  // =-= posn out of range.  throw exception?
}

//--------------------------------------------------------------
// update scrollbars
void mgSimpleList::updateScrollers()
{
  mgDimension size;
  getSize(size);
  int lineCount = size.m_height / m_lineHeight;

  // figure new range and visible, inform scrollers
  if (m_vertScroller != NULL)
  {
    m_vertScroller->setRange(0, m_entries.length());
    m_vertScroller->setView(m_scrollPosn, lineCount);
    m_vertScroller->setVisible(!m_vertScroller->isFullView());
  }
  if (m_horzScroller != NULL)
  {
    // =-= update horizontal scrollbar
  }
}

//--------------------------------------------------------------
// update layout of controls
void mgSimpleList::updateLayout()
{
  mgDimension size;
  getSize(size);

  updateScrollers();

  mgDimension vertSize, horzSize;
  if (m_vertScroller != NULL && m_vertScroller->getVisible())
    m_vertScroller->preferredSize(vertSize);
  if (m_horzScroller != NULL && m_horzScroller->getVisible())
    m_horzScroller->preferredSize(horzSize);

  if (m_vertScroller != NULL)
  {
    m_vertScroller->setLocation(size.m_width-vertSize.m_width, 0);
    m_vertScroller->setSize(vertSize.m_width, size.m_height-horzSize.m_height);
  }
  
  if (m_horzScroller != NULL)
  {
    m_horzScroller->setLocation(0, size.m_height-horzSize.m_height);
    m_horzScroller->setSize(size.m_width-vertSize.m_width, horzSize.m_height);
  }
}

//--------------------------------------------------------------
// paint content of control
void mgSimpleList::paint(
  mgContext* gc) 
{
  mgDimension size;
  getSize(size);

  gc->setAlphaMode(MG_ALPHA_SET);
  mgRectangle inside(0, 0, size.m_width, size.m_height);
  const mgFrame* cntlFrame = getEnabled() ? m_upFrame : m_disFrame;
  if (cntlFrame != NULL)
  {
    cntlFrame->paintBackground(gc, 0, 0, size.m_width, size.m_height);
    cntlFrame->getInsideRect(inside);
  }

  gc->setFont(m_font);

  int ascent = m_font->getAscent();

  // =-= clip entries to within frame
  int iconSpacing = m_iconHeight/3;

  // draw entries
  gc->setAlphaMode(MG_ALPHA_MERGE);

  // paint all the entries
  int y = 0;
  for (int i = m_scrollPosn; i < m_entries.length(); i++)
  {
    mgListEntry* entry = (mgListEntry*) m_entries.getAt(i);
    const mgFrame* itemFrame = m_itemUpFrame;
    mgColor textColor(m_itemUpColor);
    if (entry->m_selected || i == m_selected)
    {
      itemFrame = m_itemDownFrame;
      textColor = m_itemDownColor;
    }
    else if (i == m_hover)
    {
      itemFrame = m_itemHoverFrame;
      textColor = m_itemHoverColor;
    }

    // draw the entry
    mgRectangle line(inside.m_x, inside.m_y+y, inside.m_width, m_lineHeight);
    if (itemFrame != NULL)
    {
      itemFrame->paintBackground(gc, inside.m_x, inside.m_y+y, inside.m_width, m_lineHeight);
      itemFrame->getInsideRect(line);
    }

    if (entry->m_icon != NULL)
    {
      gc->drawIcon(entry->m_icon, line.m_x + iconSpacing, 
                   line.m_y + (m_lineHeight-m_iconHeight)/2, 
                   m_iconHeight, m_iconHeight);
    }

    int x = 0;
    if (m_hasIcons)
      x += iconSpacing + m_iconHeight + iconSpacing;
      
    gc->setTextColor(textColor);
    gc->drawString(entry->m_text, entry->m_text.length(), line.m_x + x, line.m_y + ascent);

    if (itemFrame != NULL)
      itemFrame->paintForeground(gc, inside.m_x, inside.m_y+y, inside.m_width, m_lineHeight);

    y += m_lineHeight;

    // if off bottom of control
    if (y >= inside.m_height)
      break;
  }

  if (cntlFrame != NULL)
    cntlFrame->paintForeground(gc, 0, 0, size.m_width, size.m_height);
}

//--------------------------------------------------------------
// get minimum size
void mgSimpleList::minimumSize(
  mgDimension& size) 
{
  preferredSize(size);
}

//--------------------------------------------------------------
// get preferred size
void mgSimpleList::preferredSize(
  mgDimension& size) 
{
  int iconSpacing = m_iconHeight/3;

  size.m_height = m_displayLines * m_lineHeight;
  size.m_width = m_font->stringWidth("nnnnnnnn", 8);  // min width
  
  for (int i = 0; i < m_entries.length(); i++)
  {
    mgListEntry* entry = (mgListEntry*) m_entries.getAt(i);

    if (!entry->m_text.isEmpty())
      size.m_width = max(size.m_width, m_font->stringWidth(entry->m_text, entry->m_text.length()));
  }
  // add room for icon
  if (m_hasIcons)
    size.m_width += iconSpacing + m_iconHeight + iconSpacing;

  if (m_itemUpFrame != NULL)
  {
    mgDimension outside;
    m_itemUpFrame->getOutsideSize(size, outside);
    // frame height already added to m_lineHeight, so just take width
    size.m_width = outside.m_width;
  }

  // add cntl frame
  if (m_upFrame != NULL)
  {
    mgDimension outside;
    m_upFrame->getOutsideSize(size, outside);
    size = outside;
  }

  // add scrollbars
  mgDimension vertSize, horzSize;
  if (m_vertScroller != NULL)
  {
    m_vertScroller->preferredSize(vertSize);
    size.m_width += vertSize.m_width;
  }

  if (m_horzScroller != NULL)
  {
    m_horzScroller->preferredSize(horzSize);
    size.m_height += horzSize.m_height;
  }
}

//--------------------------------------------------------------
// mouse entered
void mgSimpleList::mouseEnter(
  void* source,
  int x,
  int y)
{
  mouseMove(source, x, y, 0);
  damage();
}

//--------------------------------------------------------------
// mouse exited
void mgSimpleList::mouseExit(
  void* source)
{
  m_hover = -1;
  damage();
}

//--------------------------------------------------------------
// mouse pressed
void mgSimpleList::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
//  takeKeyFocus();

  mgDimension size;
  getSize(size);

  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (m_upFrame != NULL)
    m_upFrame->getInsideRect(inside);

  // =-= select entry under point
  y = y - inside.m_y;

  int index = (y/m_lineHeight) + m_scrollPosn;
  if (index < m_entries.length())
  {
    if (m_multiSelect)
    {
      // =-= shift select, cntl select, start of drag select
      // =-= reset all other selections if just click
      mgListEntry* entry = (mgListEntry*) m_entries.getAt(index);
      entry->m_selected = true;
    }
    else
    {
      m_selected = index;
    }
    damage();
  }
}

//--------------------------------------------------------------
// mouse released
void mgSimpleList::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // =-= if selection changed, notify listeners
  if (m_multiSelect)
  {
  }
  else
  {
    if (m_selected != -1)
    {
      // =-= should deselect old value
      mgListEntry* entry = (mgListEntry*) m_entries.getAt(m_selected);
      //=-= sendSelectEvent(mgSelectOnEvent, entry->m_name);
    }
  }
}

//--------------------------------------------------------------
// mouse clicked
void mgSimpleList::mouseClick(
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
void mgSimpleList::mouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
  // =-= select entry under point
  // =-= drag select
}

//--------------------------------------------------------------
// mouse moved
void mgSimpleList::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
  mgDimension size;
  getSize(size);

  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (m_upFrame != NULL)
    m_upFrame->getInsideRect(inside);

  // find entry under point
  y = y - inside.m_y;

  int index = (y/m_lineHeight) + m_scrollPosn;
  if (index < m_entries.length())
  {
    m_hover = index;
    damage();
  }
  else m_hover = -1;
}

//--------------------------------------------------------------
// line up
void mgSimpleList::guiScrollLineUp(
  void* source)
{
  if (source == (mgScroller*) m_vertScroller)
  {
    m_scrollPosn = max(0, m_scrollPosn-1);
    updateScrollers();
  }
  damage();
}

//--------------------------------------------------------------
// line down
void mgSimpleList::guiScrollLineDown(
  void* source)
{
  if (source == (mgScroller*) m_vertScroller)
  {
    mgDimension size;
    getSize(size);
    
    mgRectangle inside(0, 0, size.m_width, size.m_height);

    int viewLines = inside.m_height / m_lineHeight;
    int maxPosn = max(0, m_entries.length() - viewLines);
    m_scrollPosn = min(maxPosn, m_scrollPosn+1);
    updateScrollers();
  }
  else if (source == (mgScroller*) m_horzScroller)
  {
  }
  damage();
}

//--------------------------------------------------------------
// page up
void mgSimpleList::guiScrollPageUp(
  void* source)
{
  if (source == (mgScroller*) m_vertScroller)
  {
    mgDimension size;
    getSize(size);

    mgRectangle inside(0, 0, size.m_width, size.m_height);

    int viewLines = inside.m_height / m_lineHeight;
    m_scrollPosn = max(0, m_scrollPosn-viewLines);
    updateScrollers();
  }
  else if (source == (mgScroller*) m_horzScroller)
  {
  }
  damage();
}

//--------------------------------------------------------------
// page down
void mgSimpleList::guiScrollPageDown(
  void* source)
{
  if (source == (mgScroller*) m_vertScroller)
  {
    mgDimension size;
    getSize(size);

    mgRectangle inside(0, 0, size.m_width, size.m_height);

    int viewLines = inside.m_height / m_lineHeight;
    int maxPosn = max(0, m_entries.length() - viewLines);
    m_scrollPosn = min(maxPosn, m_scrollPosn+viewLines);
    updateScrollers();
  }
  else if (source == (mgScroller*) m_horzScroller)
  {
  }
  damage();
}

//--------------------------------------------------------------
// set position
void mgSimpleList::guiScrollSetPosition(
  void* source,
  int value)
{
  // =-= set scroll position, constrain by 0 to size-visibleLines
  if (source == (mgScroller*) m_vertScroller)
  {
    mgDimension size;
    getSize(size);

    mgRectangle inside(0, 0, size.m_width, size.m_height);

    int viewLines = inside.m_height / m_lineHeight;
    int maxPosn = max(0, m_entries.length() - viewLines);
    m_scrollPosn = min(maxPosn, max(0, value));
    updateScrollers();
  }
  else if (source == (mgScroller*) m_horzScroller)
  {
  }
  damage();
}

//--------------------------------------------------------------
// focus gained
void mgSimpleList::guiFocusGained(
  void* source)
{
}
    
//--------------------------------------------------------------
// focus lost
void mgSimpleList::guiFocusLost(
  void* source)
{
}

