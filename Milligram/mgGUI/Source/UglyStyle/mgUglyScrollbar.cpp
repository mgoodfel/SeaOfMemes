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
#include "UglyStyle/mgUglyScrollbar.h"
#include "UglyStyle/mgUglyStyle.h"

#define PART_NONE            0 
#define PART_LINEUP          1 
#define PART_PAGEUP          2 
#define PART_SLIDER          3 
#define PART_PAGEDOWN        4 
#define PART_LINEDOWN        5 

#define FIRST_INTERVAL        500  // milliseconds
#define REPEAT_INTERVAL       50   // milliseconds

//--------------------------------------------------------------
// constructor
mgUglyScrollbar::mgUglyScrollbar(
  mgControl* parent,
  const char* cntlName)
: mgScrollbarControl(parent, cntlName)
{
  m_horizontal = false;
  m_minRange = 0;
  m_lenRange = 50;
  m_minView = 20;
  m_lenView = 10;
  m_pressPart = PART_NONE;
  m_hoverPart = PART_NONE;

  mgStyle* style = getStyle();

  // assume style has set all our format elements
  style->getFontAttr(MG_STYLE_SCROLLBAR, m_cntlName, "font", m_font);

  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "upFrame", m_upFrame);
  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "disFrame", m_disFrame);

  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "lineUpFrame", m_lineUpFrame);
  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "lineHoverFrame", m_lineHoverFrame);
  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "lineDownFrame", m_lineDownFrame);

  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "pageUpFrame", m_pageUpFrame);
  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "pageHoverFrame", m_pageHoverFrame);
  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "pageDownFrame", m_pageDownFrame);

  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "sliderUpFrame", m_sliderUpFrame);
  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "sliderHoverFrame", m_sliderHoverFrame);
  style->getFrameAttr(MG_STYLE_SCROLLBAR, m_cntlName, "sliderDownFrame", m_sliderDownFrame);

  // =-= get the icons
  m_upIcon = NULL;
  m_downIcon = NULL;
  m_leftIcon = NULL;
  m_rightIcon = NULL;
  m_vslideIcon = NULL;
  m_hslideIcon = NULL;

  m_scrollListeners = NULL;
 
  addMouseListener(this);
}

//--------------------------------------------------------------
// destructor
mgUglyScrollbar::~mgUglyScrollbar()
{
  delete m_scrollListeners;
  m_scrollListeners = NULL;
}

//--------------------------------------------------------------
// set horizontal scroll bar
void mgUglyScrollbar::setHorizontal(
  BOOL horizontal)
{
  m_horizontal = horizontal;
}

//--------------------------------------------------------------
// draw frame and icon for a scrollbar part
void mgUglyScrollbar::paintPart(
  mgContext* gc,
  const mgFrame* frame,
  const mgIcon* icon,
  const mgRectangle& rect)
{
  if (frame != NULL)
    frame->paintBackground(gc, rect.m_x, rect.m_y, rect.m_width, rect.m_height);
    
  // =-= get icon bounds within frame
  if (icon != NULL)
    gc->drawIcon(icon, rect.m_x, rect.m_y, rect.m_width, rect.m_height);
  
  if (frame != NULL)
    frame->paintForeground(gc, rect.m_x, rect.m_y, rect.m_width, rect.m_height);
}
  
//--------------------------------------------------------------
// paint content of control
void mgUglyScrollbar::paint(
  mgContext* gc) 
{
  mgDimension size;
  getSize(size);
  
  mgRectangle inside(0, 0, size.m_width, size.m_height);
  const mgFrame* cntlFrame = getEnabled() ? m_upFrame : m_disFrame;
  if (cntlFrame != NULL)
  {
    cntlFrame->paintBackground(gc, 0, 0, size.m_width, size.m_height);
    cntlFrame->getInsideRect(inside);
  }

  const mgFrame* frame = NULL;
  gc->setFont(m_font);
  
  int iconHeight = m_font->getHeight();

  // constrain view to range
  int maxRange = m_minRange + m_lenRange;
  int minView = max(m_minView, m_minRange);
  int maxView = min(m_minView+m_lenView, maxRange);

  if (m_horizontal)
  {
    // horizontal scrollbar
    int btnSize = inside.m_height;

    // draw lineup button
    mgRectangle btnArea(inside.m_x, inside.m_y, btnSize, btnSize);
    frame = (m_pressPart == PART_LINEUP) ? m_lineDownFrame : 
            (m_hoverPart == PART_LINEUP) ? m_lineHoverFrame: m_lineUpFrame;
    paintPart(gc, frame, m_leftIcon, btnArea);

    m_lineUpCoord = inside.m_x + btnSize;

    // draw linedown button
    btnArea.m_x = inside.m_width-btnSize;
    frame = (m_pressPart == PART_LINEDOWN) ? m_lineDownFrame : 
            (m_hoverPart == PART_LINEDOWN) ? m_lineHoverFrame: m_lineUpFrame;
    paintPart(gc, frame, m_rightIcon, btnArea);

    m_pageDownCoord = btnArea.m_x;

    // draw slider
    int width = inside.m_width-(2*btnSize);

    // =-= windows scroll slider does not paint when no room for it
    int lx = (width * (minView-m_minRange))/m_lenRange;
    int hx = (width * (maxView-m_minRange))/m_lenRange;
    hx = max(hx, lx+iconHeight);

    frame = (m_pressPart == PART_SLIDER) ? m_sliderDownFrame : 
            (m_hoverPart == PART_SLIDER) ? m_sliderHoverFrame: m_sliderUpFrame;
    btnArea.m_x = inside.m_x + btnSize + lx;
    btnArea.m_width = hx-lx;
    paintPart(gc, frame, m_hslideIcon, btnArea);
    
    m_pageUpCoord = btnArea.m_x;
    m_sliderCoord = btnArea.m_x + btnArea.m_width;

    btnArea.m_x = m_lineUpCoord;
    btnArea.m_width = m_pageUpCoord-m_lineUpCoord;
    frame = (m_pressPart == PART_PAGEUP) ? m_pageDownFrame : 
            (m_hoverPart == PART_PAGEUP) ? m_pageHoverFrame: m_pageUpFrame;
    paintPart(gc, frame, NULL, btnArea); 

    btnArea.m_x = m_sliderCoord;
    btnArea.m_width = m_pageDownCoord - m_sliderCoord;
    frame = (m_pressPart == PART_PAGEDOWN) ? m_pageDownFrame : 
            (m_hoverPart == PART_PAGEDOWN) ? m_pageHoverFrame: m_pageUpFrame;
    paintPart(gc, frame, NULL, btnArea); 
  }
  else
  {
    // vertical scrollbar
    int btnSize = inside.m_width;

    // draw lineup button
    mgRectangle btnArea(inside.m_x, inside.m_y, btnSize, btnSize);
    frame = (m_pressPart == PART_LINEUP) ? m_lineDownFrame : 
            (m_hoverPart == PART_LINEUP) ? m_lineHoverFrame: m_lineUpFrame;
    paintPart(gc, frame, m_upIcon, btnArea);

    m_lineUpCoord = inside.m_y + btnSize;

    // draw linedown button
    btnArea.m_y = inside.m_height-btnSize;
    frame = (m_pressPart == PART_LINEDOWN) ? m_lineDownFrame : 
            (m_hoverPart == PART_LINEDOWN) ? m_lineHoverFrame: m_lineUpFrame;
    paintPart(gc, frame, m_downIcon, btnArea);

    m_pageDownCoord = btnArea.m_y;

    // draw slider
    int height = inside.m_height-(2*btnSize);

    // =-= constrain minvisible and maxvisible
    int ly = (height * (minView-m_minRange))/m_lenRange;
    int hy = (height * (maxView-m_minRange))/m_lenRange;
    hy = max(hy, ly+iconHeight);

    frame = (m_pressPart == PART_SLIDER) ? m_sliderDownFrame : 
            (m_hoverPart == PART_SLIDER) ? m_sliderHoverFrame: m_sliderUpFrame;
    btnArea.m_y = inside.m_y + btnSize + ly;
    btnArea.m_height = hy-ly;
    paintPart(gc, frame, m_vslideIcon, btnArea);
    
    m_pageUpCoord = btnArea.m_y; 
    m_sliderCoord = btnArea.m_y + btnArea.m_height;

    btnArea.m_y = m_lineUpCoord;
    btnArea.m_height = m_pageUpCoord-m_lineUpCoord;
    frame = (m_pressPart == PART_PAGEUP) ? m_pageDownFrame : 
            (m_hoverPart == PART_PAGEUP) ? m_pageHoverFrame: m_pageUpFrame;
    paintPart(gc, frame, NULL, btnArea); 

    btnArea.m_y = m_sliderCoord;
    btnArea.m_height = m_pageDownCoord - m_sliderCoord;
    frame = (m_pressPart == PART_PAGEDOWN) ? m_pageDownFrame : 
            (m_hoverPart == PART_PAGEDOWN) ? m_pageHoverFrame: m_pageUpFrame;
    paintPart(gc, frame, NULL, btnArea); 
  }

  if (cntlFrame != NULL)
    cntlFrame->paintForeground(gc, 0, 0, size.m_width, size.m_height);
}

//--------------------------------------------------------------
// get minimum size of control
void mgUglyScrollbar::minimumSize(
  mgDimension& size) 
{
  preferredSize(size);
}

//--------------------------------------------------------------
// get preferred size of control
void mgUglyScrollbar::preferredSize(
  mgDimension& size) 
{
  int iconHeight = m_font->getHeight();

  if (m_horizontal)
  {
    size.m_width = iconHeight * 5;
    size.m_height = iconHeight;
  }
  else
  {
    size.m_width = iconHeight;
    size.m_height = iconHeight * 5;
  }

  // add cntl frame
  if (m_upFrame != NULL)
  {
    mgDimension extSize;
    m_upFrame->getOutsideSize(size, extSize);
    size = extSize;
  }
}

//--------------------------------------------------------------
// mouse entered
void mgUglyScrollbar::mouseEnter(
  void* source,
  int x,
  int y)
{
  mouseMove(source, x, y, 0);
}

//--------------------------------------------------------------
// mouse exited
void mgUglyScrollbar::mouseExit(
  void* source)
{
  if (m_hoverPart != PART_NONE)
  {
    m_hoverPart = PART_NONE;
    damage();
  }
}

//--------------------------------------------------------------
// mouse pressed
void mgUglyScrollbar::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  int oldPress = m_pressPart;

  // find area of press
  if (m_horizontal)
  {
    if (x < m_lineUpCoord)
      m_pressPart = PART_LINEUP;
    else if (x < m_pageUpCoord)
    {
      m_dragStart = x;
      m_pressPart = PART_PAGEUP;
    }
    else if (x < m_sliderCoord)
    {
      m_pressPart = PART_SLIDER;
      m_dragOffset = x - m_pageUpCoord; // start of slider
    }
    else if (x < m_pageDownCoord)
    {    
      m_pressPart = PART_PAGEDOWN;
      m_dragStart = x;
    }
    else m_pressPart = PART_LINEDOWN;
  }
  else
  {
    if (y < m_lineUpCoord)
      m_pressPart = PART_LINEUP;
    else if (y < m_pageUpCoord)
    {
      m_pressPart = PART_PAGEUP;
      m_dragStart = y;
    }      
    else if (y < m_sliderCoord)
    {
      m_pressPart = PART_SLIDER;
      m_dragOffset = y - m_pageUpCoord; // start of slider
    }
    else if (y < m_pageDownCoord)
    {
      m_pressPart = PART_PAGEDOWN;
      m_dragStart = y;
    }     
    else m_pressPart = PART_LINEDOWN;
  }

  // do first scroll event
  switch (m_pressPart)
  {
    case PART_LINEUP:
    case PART_LINEDOWN:
    case PART_PAGEUP:
    case PART_PAGEDOWN:
      sendScrollEvent(m_pressPart, 0);
      break;
    case PART_SLIDER: ;
  }

  if (m_pressPart != PART_NONE && m_pressPart != PART_SLIDER)
  {
    // start timer
    addTimeListener((mgTimeListener*) this);  
    m_pressTime = mgOSGetTime();
    m_pressInterval = FIRST_INTERVAL;
  }

  if (m_pressPart != oldPress)
    damage();
}

//--------------------------------------------------------------
// mouse released
void mgUglyScrollbar::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  // reset press area
  m_pressPart = PART_NONE;
  damage();

  removeTimeListener((mgTimeListener*) this);  
}

//--------------------------------------------------------------
// mouse clicked
void mgUglyScrollbar::mouseClick(
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
void mgUglyScrollbar::mouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
  // if slider area
  if (m_pressPart == PART_SLIDER)
  {
    // drag slider
    if (m_horizontal)
    {
      // where new left of slider would be
      x = (x - m_dragOffset)-m_lineUpCoord;
      // range of possible x values
      int xLen = m_pageDownCoord - m_lineUpCoord;
      // value of scroller
      int value = m_minRange + (m_lenRange*x)/xLen;
      sendScrollEvent(PART_SLIDER, value);
    }
    else
    {
      // where new top of slider would be
      y = (y - m_dragOffset)-m_lineUpCoord;
      // range of possible y values
      int yLen = m_pageDownCoord - m_lineUpCoord;
      // value of scroller
      int value = m_minRange + (m_lenRange*y)/yLen;
      sendScrollEvent(PART_SLIDER, value);
    }
  }
}

//--------------------------------------------------------------
// mouse moved
void mgUglyScrollbar::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
  int oldPart = m_hoverPart;

  // find area of press
  if (m_horizontal)
  {
    if (x < m_lineUpCoord)
      m_hoverPart = PART_LINEUP;
    else if (x < m_pageUpCoord)
    {
      m_dragStart = x;
      m_hoverPart = PART_PAGEUP;
    }
    else if (x < m_sliderCoord)
    {
      m_hoverPart = PART_SLIDER;
      m_dragOffset = x - m_pageUpCoord; // start of slider
    }
    else if (x < m_pageDownCoord)
    {    
      m_hoverPart = PART_PAGEDOWN;
      m_dragStart = x;
    }
    else m_hoverPart = PART_LINEDOWN;
  }
  else
  {
    if (y < m_lineUpCoord)
      m_hoverPart = PART_LINEUP;
    else if (y < m_pageUpCoord)
    {
      m_hoverPart = PART_PAGEUP;
      m_dragStart = y;
    }      
    else if (y < m_sliderCoord)
    {
      m_hoverPart = PART_SLIDER;
      m_dragOffset = y - m_pageUpCoord; // start of slider
    }
    else if (y < m_pageDownCoord)
    {
      m_hoverPart = PART_PAGEDOWN;
      m_dragStart = y;
    }     
    else m_hoverPart = PART_LINEDOWN;
  }

  if (oldPart != m_hoverPart)
    damage();
}

//--------------------------------------------------------------
// time has passed
void mgUglyScrollbar::guiTimerTick(
  double now,
  double since)
{
  // do first scroll event
  switch (m_pressPart)
  {
    case PART_LINEUP:
    case PART_LINEDOWN:
      if (now - m_pressTime > m_pressInterval)
      {
        sendScrollEvent(m_pressPart, 0);
        m_pressTime = m_pressTime + m_pressInterval;
        m_pressInterval = REPEAT_INTERVAL;
      }
      break;

    case PART_PAGEUP:
      // if paged past cursor position, stop timer
      if (m_dragStart > m_pageUpCoord)
        removeTimeListener((mgTimeListener*) this);
        
      else if (now - m_pressTime > m_pressInterval)
      {
        sendScrollEvent(m_pressPart, 0);
        m_pressTime = m_pressTime + m_pressInterval;
        m_pressInterval = REPEAT_INTERVAL;
      }
      break;

    case PART_PAGEDOWN:
      // if paged past cursor position, stop timer
      if (m_dragStart < m_sliderCoord)
        removeTimeListener((mgTimeListener*) this);

      else if (now - m_pressTime > m_pressInterval)
      {
        sendScrollEvent(m_pressPart, 0);
        m_pressTime = m_pressTime + m_pressInterval;
        m_pressInterval = REPEAT_INTERVAL;
      }
      break;

    case PART_SLIDER: 
      // timer should not be on during slider drag
      ;
  }
}

//--------------------------------------------------------------
// add a scroll listener
void mgUglyScrollbar::addScrollListener(
  mgScrollListener* listener) 
{
  if (m_scrollListeners == NULL)
    m_scrollListeners = new mgPtrArray();
  m_scrollListeners->add(listener);
}

//--------------------------------------------------------------
// remove a scroll listener
void mgUglyScrollbar::removeScrollListener(
  mgScrollListener* listener) 
{
  if (m_scrollListeners == NULL)
    return;
  m_scrollListeners->remove(listener);
}

//--------------------------------------------------------------
// send scroll event
void mgUglyScrollbar::sendScrollEvent(
  int scrollType,
  int value)
{
  if (m_scrollListeners == NULL)
    return;
  for (int i = 0; i < m_scrollListeners->length(); i++)
  {
    mgScrollListener* listener = (mgScrollListener*) m_scrollListeners->getAt(i);
    switch (scrollType)
    {
      case PART_LINEUP:
        listener->guiScrollLineUp((mgScroller*) this);
        break;
      case PART_LINEDOWN:
        listener->guiScrollLineDown((mgScroller*) this);
        break;
      case PART_PAGEUP:
        listener->guiScrollPageUp((mgScroller*) this);
        break;
      case PART_PAGEDOWN:
        listener->guiScrollPageDown((mgScroller*) this);
        break;
      case PART_SLIDER:
        listener->guiScrollSetPosition((mgScroller*) this, value);
        break;
    }
  }
}

//--------------------------------------------------------------
// set the range of the scroller
void mgUglyScrollbar::setRange(
  int minRange,
  int lenRange) 
{
  // handle empty content (len=0)
  m_minRange = minRange;
  m_lenRange = max(1, lenRange);
  damage();
}

//--------------------------------------------------------------
// set the view of the scroller
void mgUglyScrollbar::setView(
  int minView,
  int lenView) 
{
  // handle empty view
  m_minView = minView;
  m_lenView = max(1, lenView);
  damage();
}

//--------------------------------------------------------------
// get the range of the scroller
void mgUglyScrollbar::getRange(
  int& minRange,
  int& lenRange) 
{
  minRange = m_minRange;
  lenRange = m_lenRange;
}

//--------------------------------------------------------------
// get the view position
void mgUglyScrollbar::getView(
  int& minView,
  int& lenView) 
{
  minView = m_minView;
  lenView = m_lenView;
}

//--------------------------------------------------------------
// return true if view is entire range
BOOL mgUglyScrollbar::isFullView() 
{
  // if view covers entire range
  return m_minView <= m_minRange &&
         (m_minView + m_lenView) >= (m_minRange + m_lenRange);
}

