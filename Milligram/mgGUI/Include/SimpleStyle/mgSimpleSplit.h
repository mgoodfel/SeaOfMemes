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
#ifndef MGSIMPLESPLIT_H
#define MGSIMPLESPLIT_H

#include "GUI/mgSplitControl.h"
#include "GUI/mgMouseListener.h"

class mgFrame;

/*
  A horizontal or vertical split pane with draggable separator between
  two child controls.
*/
class mgSimpleSplit : public mgSplitControl, public mgMouseListener
{
public:
  // constructor
  mgSimpleSplit(
    mgControl* parent,
    const char* cntlName = NULL);

  // destructor
  virtual ~mgSimpleSplit();

  // set vertical or horizontal split
  virtual void setVertical(
    BOOL vertical);
  
  // create a new pane in the split
  virtual mgControl* addPane(
    int weight);

  // resize the panes based on their weights
  virtual void resizePanes();

  virtual void minimumSize(
    mgDimension& size);

  virtual void preferredSize(
    mgDimension& size);
    
protected:
  const mgFont* m_font;                 // font used
  const mgFrame* m_divider;             // frame which draws divider
//  const mgCursor* m_vertCursor;         // vertical split cursor
//  const mgCursor* m_horzCursor;         // horz split cursor
  
  BOOL m_vertical;                      // true if a vertical split (dividers are vertical)
  int m_dividerWidth;                   // size of divider between children
  mgPtrArray m_panes;                   // child panes to move

  int m_dragIndex;                      // pane being dragged
  int m_dragX;                          // start drag coordinate
  int m_dragY;
  
  const mgFrame* m_defaultFrame;        // default framing
  
  // paint content of control
  virtual void paint(
    mgContext* gc);

  // return index of pane divider under point
  virtual int findDivider(
    int cursorX,
    int cursorY);

  // update child layout after resize
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

};

#endif
