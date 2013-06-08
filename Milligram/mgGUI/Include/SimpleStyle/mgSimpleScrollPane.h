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
#ifndef MGSIMPLESCROLLPANE_H
#define MGSIMPLESCROLLPANE_H

#include "GUI/mgScrollPaneControl.h"
#include "GUI/mgScrollListener.h"

class mgScrollbarControl;

/*
  A pane with scroll bars
*/
class mgSimpleScrollPane: public mgScrollPaneControl, mgScrollListener
{
public:
  // constructor
  mgSimpleScrollPane(
    mgControl* parent,
    const char* cntlName = NULL);
    
  // destructor
  virtual ~mgSimpleScrollPane();
  
  // get scrolled pane parent
  virtual mgControl* getScrollParent();
    
  // set new vertical scrollbar
  virtual void setVScrollbar(
    mgScrollbarControl* scrollbar);

  // set new horizontal scroller
  virtual void setHScrollbar(
    mgScrollbarControl* scrollbar);

  virtual void minimumSize(
    mgDimension& size);

  virtual void preferredSize(
    mgDimension& size);
    
protected:
  mgControl* m_paneParent;                        // parent to clip pane 
  int m_lineHeight;                               // per-line scroll amount
  mgScrollbarControl* m_vertScrollbar;            // vertical scroll bar, if any
  mgScrollbarControl* m_horzScrollbar;            // horizontal scroll bar, if any

  // update scrollbars
  virtual void updateScrollers();

  // update layout of children
  virtual void updateLayout();

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
