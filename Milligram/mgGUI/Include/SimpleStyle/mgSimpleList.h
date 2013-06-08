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
#ifndef MGSIMPLELIST_H
#define MGSIMPLELIST_H

#include "GUI/mgListControl.h"
#include "GUI/mgMouseListener.h"
#include "GUI/mgFocusListener.h"
#include "GUI/mgScrollListener.h"
#include "GUI/mgSelectListener.h"

class mgSimpleScrollbar;
class mgFrame;
class mgPaint;

/*
  A text list control with vertical scollbar.
*/
class mgSimpleList: public mgListControl,
  public mgMouseListener, public mgFocusListener, public mgScrollListener
{
public:
  // constructor
  mgSimpleList(
    mgControl* parent,
    const char* cntlName = NULL);
    
  // destructor
  virtual ~mgSimpleList();
  
  // set number of lines to display
  virtual void setDisplayLines(
    int displayLines)
  {
    m_displayLines = displayLines;
  }

  // add an entry with text and icon
  virtual void addEntry(
    const char* name,
    const char* text,
    const mgIcon* icon = NULL);

  // true if supports multiselect
  virtual BOOL canMultiSelect()
  { 
    return true; 
  }

  // set multiple select mode
  virtual void setMultiSelect(
    BOOL multiSelect);

  // set item selected
  virtual void setItemState(
    const char* name,
    BOOL selected);

  // return true if item selected
  virtual BOOL getItemState(
    const char* name);

  // get count of selected items
  virtual int getSelectionCount();
    
  // get nth selected item
  virtual void getSelectedItem(
    int posn,
    mgString &name);
    
  // paint content of control
  virtual void paint(
    mgContext* gc);

  virtual void minimumSize(
    mgDimension& size);

  virtual void preferredSize(
    mgDimension& size);
    
  // add a select listener
  virtual void addSelectListener(
    mgSelectListener* listener);

  // remove a select listener
  virtual void removeSelectListener(
    mgSelectListener* listener);

protected:
  const mgFont* m_font;
  const mgFrame* m_upFrame;
  const mgFrame* m_disFrame;

  mgColor m_itemUpColor;
  mgColor m_itemHoverColor;
  mgColor m_itemDownColor;
  mgColor m_itemDisColor;
  const mgFrame* m_itemUpFrame;
  const mgFrame* m_itemHoverFrame;
  const mgFrame* m_itemDownFrame;
  const mgFrame* m_itemDisFrame;

  mgPtrArray m_entries;                   // list entries.  of Entry
  BOOL m_hasIcons;                        // true if there are icons

  int m_lineHeight;                       // height of lines
  int m_iconHeight;                       // height of icons

  int m_displayLines;                     // number of lines to display
  BOOL m_multiSelect;                     // multiple selections allowed

  int m_selected;                         // current selection, if single
  int m_hover;                            // item under cursor
  int m_scrollPosn;                       // first line to write

  mgSimpleScrollbar* m_vertScroller;             // vertical scroll bar, if any
  mgSimpleScrollbar* m_horzScroller;             // horizontal scroll bar, if any

  mgPtrArray* m_selectListeners;            // listeners for select events

#ifdef WORKED
  // send event to select listeners
  virtual void sendSelectEvent(
    mgSelectEventType type,
    const char* value);
#endif

  // update scrollbars
  virtual void updateScrollers();

  // update layout of children
  virtual void updateLayout();

  // mouse entered
  virtual void mouseEnter(
    void* source,
    int x,
    int y);

  // mouse exited
  virtual void mouseExit(
    void* source);

  // mouse pressed
  virtual void mouseDown(
    void* source,
    int x,
    int y,
    int modifiers,
    int button);

  // mouse released
  virtual void mouseUp(
    void* source,
    int x,
    int y,
    int modifiers,
    int button);

  // mouse clicked
  virtual void mouseClick(
    void* source,
    int x,
    int y,
    int modifiers,
    int button,
    int clickCount);

  // mouse dragged
  virtual void mouseDrag(
    void* source,
    int x,
    int y,
    int modifiers);

  // mouse moved
  virtual void mouseMove(
    void* source,
    int x,
    int y,
    int modifiers);

  // focus gained
  virtual void guiFocusGained(
    void* source);
    
  // focus lost
  virtual void guiFocusLost(
    void* source);

  // line up
  virtual void guiScrollLineUp(
    void* source);

  // line down
  virtual void guiScrollLineDown(
    void* source);

  // page up
  virtual void guiScrollPageUp(
    void* source);

  // page down
  virtual void guiScrollPageDown(
    void* source);

  // set position
  virtual void guiScrollSetPosition(
    void* source,
    int value);
};

#endif
