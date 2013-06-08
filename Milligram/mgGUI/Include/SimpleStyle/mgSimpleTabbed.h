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
#ifndef MGSIMPLETABBED_H
#define MGSIMPLETABBED_H

#include "GUI/mgTabbedControl.h"
#include "GUI/mgMouseListener.h"

class mgFrame;

/*
  A tabbed page control.  Selecting a tab shows one of the child controls.
*/
class mgSimpleTabbed : public mgTabbedControl, public mgMouseListener
{
public:
  // constructor
  mgSimpleTabbed(
    mgControl* parent,
    const char* cntlName = NULL);
    
  // destructor
  virtual ~mgSimpleTabbed();
  
  // add a pane to the stack
  virtual mgControl* addPane(
    const char* label,
    const char* name);
    
  // remove a pane from the stack
  virtual void removePane(
    const char* name);
    
  // select pane to show
  virtual void selectPane(
    const char* name);
    
  // get selected pane
  virtual void getSelected(
    mgString& name);
    

  // get minimum size of control
  virtual void minimumSize(
    mgDimension& size);

  // get preferred size of control
  virtual void preferredSize(
    mgDimension& size);

protected:
  const mgFont* m_font;
  mgColor m_upColor;
  mgColor m_hoverColor;
  mgColor m_downColor;
  mgColor m_disColor;
  const mgFrame* m_childFrame;
  const mgFrame* m_upFrame;
  const mgFrame* m_hoverFrame;
  const mgFrame* m_downFrame;
  const mgFrame* m_disFrame;
  
  mgPtrArray m_tabs;
  
  int m_tabHeight;
  BOOL m_hover;

  // update layout after resize
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

  // paint content of control
  virtual void paint(
    mgContext* gc);
};

#endif
