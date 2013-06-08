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
#ifndef MGBARSCROLL_H
#define MGBARSCROLL_H

#include "GUI/mgScrollbarControl.h"
#include "GUI/mgScroller.h"

#include "GUI/mgMouseListener.h"
#include "GUI/mgScrollListener.h"
#include "GUI/mgTimeListener.h"

class mgFrame;

/*
  A simple scroll bar control.
*/
class mgSimpleScrollbar : public mgScrollbarControl, public mgMouseListener, public mgTimeListener
{
public:
  // constructor
  mgSimpleScrollbar(
    mgControl* parent,
    const char* cntlName = NULL);
    
  // destructor
  virtual ~mgSimpleScrollbar();
  
  // set horizontal scrollbar
  virtual void setHorizontal(
    BOOL horiztonal);
    
  // add a scroll listener
  virtual void addScrollListener(
    mgScrollListener* listener);

  // remove a scroll listener
  virtual void removeScrollListener(
    mgScrollListener* listener);

  // set the range of the scroller
  virtual void setRange(
    int minRange,
    int lenRange);

  // get the range of the scroller
  virtual void getRange(
    int& minRange,
    int& lenRange);

  // set the view of the scroller
  virtual void setView(
    int minView,
    int lenView);

  // get the view position
  virtual void getView(
    int& minView,
    int& lenView);

  // return true if view is entire range
  virtual BOOL isFullView();

  // get minimum size of control
  virtual void minimumSize(
    mgDimension& size);

  // get preferred size of control
  virtual void preferredSize(
    mgDimension& size);
    
protected:
  const mgFont* m_font;

  const mgIcon* m_upIcon;
  const mgIcon* m_downIcon;
  const mgIcon* m_leftIcon;
  const mgIcon* m_rightIcon;
  const mgIcon* m_vslideIcon;
  const mgIcon* m_hslideIcon;

  const mgFrame* m_upFrame;
  const mgFrame* m_disFrame;

  const mgFrame* m_lineUpFrame;
  const mgFrame* m_lineHoverFrame;
  const mgFrame* m_lineDownFrame;

  const mgFrame* m_pageUpFrame;
  const mgFrame* m_pageHoverFrame;
  const mgFrame* m_pageDownFrame;

  const mgFrame* m_sliderUpFrame;
  const mgFrame* m_sliderHoverFrame;
  const mgFrame* m_sliderDownFrame;

  BOOL m_horizontal;                    // horizontal scroller
  int m_minRange;                       // min range
  int m_lenRange;                       // length of range
  int m_minView;                        // min view
  int m_lenView;                        // length of view

  // coordinates of hit areas, from paint
  int m_lineUpCoord;
  int m_pageUpCoord;
  int m_sliderCoord;
  int m_pageDownCoord;

  int m_pressPart;                      // part pressed
  int m_hoverPart;                      // part under cursor
  int m_dragStart;                      // starting coord of drag
  int m_dragOffset;                     // offset of start from slider
  double m_pressTime;                   // start of button press
  double m_pressInterval;               // how long for press
  
  mgPtrArray* m_scrollListeners;        // scroll event listeners

  // send scroll event to listeners
  void sendScrollEvent(
    int scrollType,
    int value);

  // draw frame and icon for a scrollbar part
  void paintPart(
    mgContext* gc,
    const mgFrame* frame,
    const mgIcon* icon,
    const mgRectangle& rect);

  // paint content of control
  virtual void paint(
    mgContext* gc);

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

  // time has passed
  virtual void guiTimerTick(
    double now,
    double since);
};

#endif
