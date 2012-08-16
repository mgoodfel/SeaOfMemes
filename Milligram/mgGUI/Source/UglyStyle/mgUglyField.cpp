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
#include "mgInputEvents.h"
#include "UglyStyle/mgUglyField.h"
#include "UglyStyle/mgUglyStyle.h"

// Note: cursorPosn and scrollPosn are "letter" positions in the displayed text.

//--------------------------------------------------------------
// constructor
mgUglyField::mgUglyField(
  mgControl* parent,
  const char* cntlName)
: mgFieldControl(parent, cntlName)
{
  m_displayCount = 20;  // default
  m_cursorPosn = 0;
  m_scrollPosn = 0;
  m_insertMode = true;
  m_changed = false;
  m_history = NULL;  // not enabled

  mgStyle* style = getStyle();

  // assume style has set all our format elements
  style->getFontAttr(MG_STYLE_FIELD, m_cntlName, "font", m_font);

  style->getFrameAttr(MG_STYLE_FIELD, m_cntlName, "upFrame", m_upFrame);
  style->getFrameAttr(MG_STYLE_FIELD, m_cntlName, "hoverFrame", m_hoverFrame);
  style->getFrameAttr(MG_STYLE_FIELD, m_cntlName, "downFrame", m_downFrame);
  style->getFrameAttr(MG_STYLE_FIELD, m_cntlName, "disFrame", m_disFrame);

  style->getColorAttr(MG_STYLE_FIELD, m_cntlName, "upColor", m_upColor);
  style->getColorAttr(MG_STYLE_FIELD, m_cntlName, "hoverColor", m_hoverColor);
  style->getColorAttr(MG_STYLE_FIELD, m_cntlName, "downColor", m_downColor);
  style->getColorAttr(MG_STYLE_FIELD, m_cntlName, "disColor", m_disColor);

  m_hasMouse = false;

  m_changeListeners = NULL;
  addMouseListener((mgMouseListener*) this);
  addKeyListener((mgKeyListener*) this);
  addFocusListener((mgFocusListener*) this);
}

//--------------------------------------------------------------
// destructor
mgUglyField::~mgUglyField()
{
  delete m_changeListeners;
  m_changeListeners = NULL;

  delete m_history;
  m_history = NULL;
}

//--------------------------------------------------------------
// enable input history
void mgUglyField::enableHistory(
  BOOL enable)
{
  if (enable)
  {
    if (m_history == NULL)
    {
      m_history = new mgStringArray();
      m_historyPosn = 0;
    }
    // otherwise already enabled
  }
  else 
  {
    delete m_history;
    m_history = NULL;
  }
}

//--------------------------------------------------------------
// set number of chars to display
void mgUglyField::setDisplayCount(
  int count)
{
  m_displayCount = count;
}

//--------------------------------------------------------------
// set text in field
void mgUglyField::setText(
  const char* text)
{
  m_text = text;
  m_cursorPosn = 0;
  m_scrollPosn = 0;
  m_changed = false;

  damage();
}

//--------------------------------------------------------------
// get value in field
void mgUglyField::getText(
  mgString& text)
{
  text = m_text;
}

//--------------------------------------------------------------
// reset the field
void mgUglyField::reset()
{
  m_text.empty();
  m_cursorPosn = 0;
  m_scrollPosn = 0;

  damage();
}

//--------------------------------------------------------------
// get cursor X coordinate
int mgUglyField::getCursorX(
  const mgString& displayStr)
{
  int cursorX = 0;
  int posn = m_cursorPosn - m_scrollPosn;
  
  // if cursor position within displayed text
  if (posn <= displayStr.length())
  {
    // measure string from scroll position up to cursor
    cursorX = m_font->stringWidth(displayStr, posn);
  }
  else
  {
    // measure string from scroll position, plus blanks to cursor position
    cursorX = m_font->stringWidth(displayStr, displayStr.length());
    cursorX += m_font->stringWidth(" ", 1) * (posn - displayStr.length());
  }
  
  return cursorX;
}

//--------------------------------------------------------------
// adjust scroll posn after change in cursor posn
void mgUglyField::updateScrollPosn()
{
  // scroll backwards if off front end
  while (m_cursorPosn < m_scrollPosn+2 && m_scrollPosn > 0)
  {
    m_scrollPosn = m_text.prevLetter(m_scrollPosn);
  }
   
  // scroll forwards if off back end
  int blankWidth = m_font->stringWidth(" ", 1);

  while (true)
  {
    mgString fitText(m_text);
    fitText.deleteAt(0, m_scrollPosn);
    int cursorX = getCursorX(m_text);
    if (cursorX < (m_width-2*blankWidth))
      break;
    m_scrollPosn = m_text.nextLetter(m_scrollPosn);
  }
}

//--------------------------------------------------------------
// return minimum size of control
void mgUglyField::minimumSize(
  mgDimension& size) 
{
  preferredSize(size);
}

//--------------------------------------------------------------
// return preferred size of control
void mgUglyField::preferredSize(
  mgDimension& size) 
{
  mgSurface* surface = getSurface();

  size.m_height = m_font->getHeight();
  size.m_width = m_font->stringWidth("n", 1) * m_displayCount;

  if (m_upFrame != NULL)
  {
    mgDimension exterior;
    m_upFrame->getOutsideSize(size, exterior);
    size = exterior;
  }
}  

//--------------------------------------------------------------
// get current frame and text color from state
void mgUglyField::getFrame(
  const mgFrame*& frame,
  mgColor& textColor)
{
  if (getEnabled())
  {
    if (isKeyFocus())
    {
      frame = m_downFrame;
      textColor = m_downColor;
    }
    else if (m_hasMouse)
    {
      frame = m_hoverFrame;
      textColor = m_hoverColor;
    }
    else
    {
      frame = m_upFrame;
      textColor = m_upColor;
    }
  }
  else
  {
    frame = m_disFrame;
    textColor = m_disColor;
  }
}

//--------------------------------------------------------------
// paint content of control
void mgUglyField::paint(
  mgContext* gc)
{
  // select frame based on state
  const mgFrame* frame = NULL;
  mgColor textColor;
  getFrame(frame, textColor);

  mgSurface* surface = getSurface();
  gc->setFont(m_font);

  gc->setDrawMode(MG_DRAW_ALL);

  // draw the background
  mgDimension size;
  getSize(size);
  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (frame != NULL)
  {
    frame->paintBackground(gc, 0, 0, size.m_width, size.m_height);
    frame->getInsideRect(inside);
  }
  
  // shouldn't happen, but make sure cursor not off left edge
  m_scrollPosn = min(m_scrollPosn, m_cursorPosn);
  
  // get visible portion of text
  mgString displayText(m_text);
  displayText.deleteAt(0, m_scrollPosn);
  
  int displayLen = m_font->stringFit(displayText, displayText.length(), m_width);
  int ascent = m_font->getAscent();
  int lineHeight = m_font->getHeight();
  int y = (inside.m_height - lineHeight)/2;
  
//  mgRectangle bounds;
//  mgPoint endPt;
//  m_overlay->stringExtent(displayText, displayLen, endPt, bounds);

  gc->setDrawMode(MG_DRAW_RGB);
  gc->setAlpha(255);
  gc->setTextColor(textColor);
  gc->drawString(displayText, displayLen, inside.m_x, inside.m_y+y+ascent);

  if (isKeyFocus())
  {
    // draw the cursor
    int cursorX = getCursorX(displayText);

    gc->setDrawMode(MG_DRAW_ALL);
    gc->setAlpha(255);
    gc->setBrush(surface->createBrush(textColor));
    if (m_insertMode)
    {
      // draw cursor, 2-pixel wide line
      gc->fillRect(inside.m_x+cursorX, inside.m_y+y, 2, lineHeight);
    }
    else
    {
      // draw block cursor
      char underCursor[MG_MAX_LETTER];
      displayText.nextLetter(m_cursorPosn - m_scrollPosn, underCursor);

      int cursorWidth = m_font->stringWidth(underCursor, strlen(underCursor));
      gc->drawRect(inside.m_x+cursorX, inside.m_y+y, cursorWidth, lineHeight);
    }
  }

  if (frame != NULL)
    frame->paintForeground(gc, 0, 0, size.m_width, size.m_height);
}

//--------------------------------------------------------------
// add a change listener
void mgUglyField::addChangeListener(
  mgChangeListener* listener)
{
  if (m_changeListeners == NULL)
    m_changeListeners = new mgPtrArray();
  m_changeListeners->add(listener);
}

//--------------------------------------------------------------
// remove a change listener
void mgUglyField::removeChangeListener(
  mgChangeListener* listener)
{
  if (m_changeListeners == NULL)
    return;
  m_changeListeners->remove(listener);
}

//--------------------------------------------------------------
// send event to change listeners
void mgUglyField::sendChangeEvent()
{
  if (m_changeListeners == NULL)
    return;

  for (int i = 0; i < m_changeListeners->length(); i++)
  {
    mgChangeListener* listener = (mgChangeListener*) m_changeListeners->getAt(i);
    listener->guiChange(this, m_cntlName);
  }
}

//--------------------------------------------------------------
// key pressed
void mgUglyField::keyDown(
  void* source,
  int keyCode,
  int modifiers)
{
  switch (keyCode)
  {
    case MG_EVENT_KEY_RETURN:
      // add input to history
      if (m_history != NULL)
      {
        m_history->add(m_text);
        m_historyPosn = m_history->length()-1;
      }
      sendChangeEvent();
      break;

    case MG_EVENT_KEY_INSERT:
      // toggle insert mode
      m_insertMode = !m_insertMode;
      break;

    case MG_EVENT_KEY_DELETE:
      // delete at cursor
      if (m_cursorPosn < m_text.length())
      {
        m_text.deleteLettersAt(m_cursorPosn, 1);
        m_changed = true;
      }
      break;

    case MG_EVENT_KEY_BACKSPACE:
      // delete before cursor
      if (m_cursorPosn > 0)
      {
        if (m_cursorPosn <= m_text.length())
        {
          m_cursorPosn = m_text.prevLetter(m_cursorPosn);
          m_text.deleteLettersAt(m_cursorPosn, 1);
          m_changed = true;
        }
        else m_cursorPosn--;  // cursor beyond end of text

        updateScrollPosn();
      }
      break;

    case MG_EVENT_KEY_LEFT:
      // move cursor
      m_cursorPosn = m_text.prevLetter(m_cursorPosn);
      m_cursorPosn = max(0, m_cursorPosn);
      updateScrollPosn();
      break;

    case MG_EVENT_KEY_RIGHT:
      // move cursor
      m_cursorPosn = m_text.nextLetter(m_cursorPosn);
      updateScrollPosn();
      break;

    case MG_EVENT_KEY_UP:
      if (m_history != NULL && m_history->length() > 0)
      {
        setText(m_history->getAt(m_historyPosn));
        m_historyPosn = max(0, m_historyPosn-1);
      }
      break;

    case MG_EVENT_KEY_DOWN:
      if (m_history != NULL && m_history->length() > 0)
      {
        m_historyPosn = min(m_history->length()-1, m_historyPosn+1);
        setText(m_history->getAt(m_historyPosn));
      }
      break;

    case MG_EVENT_KEY_HOME:
      // move cursor to start
      m_cursorPosn = 0;
      updateScrollPosn();
      break;

    case MG_EVENT_KEY_END:
      // move cursor to end
      m_cursorPosn = m_text.length();
      updateScrollPosn();
      break;

    default:
      return;  // skip damage
  }
  damage();
}

//--------------------------------------------------------------
// key released
void mgUglyField::keyUp(
  void* source,
  int key,
  int modifiers)
{
}

//--------------------------------------------------------------
// character typed
void mgUglyField::keyChar(
  void* source,
  int keyCode,
  int modifiers)
{
  if (!iswprint(keyCode) && keyCode != ' ')
    return;  // non-letter key

  WCHAR keyChar[2];
  keyChar[0] = (WCHAR) keyCode;
  keyChar[1] = '\0';
  mgString letter(keyChar);

  if (m_insertMode)
  {
    // extend string up to cursor position
    while (m_text.length() < m_cursorPosn)
      m_text += " ";

    // insert character at cursor
    m_text.insertAt(m_cursorPosn, letter);
  }
  else
  {
    // extend string, including cursor position
    while (m_text.length() <= m_cursorPosn)
      m_text += " ";

    // replace current character
    m_text.setLetter(m_cursorPosn, letter);
  }
  m_cursorPosn = m_text.nextLetter(m_cursorPosn);
  updateScrollPosn();
  m_changed = true;

  damage();
}

//--------------------------------------------------------------
// mouse entered
void mgUglyField::mouseEnter(
  void* source,
  int x,
  int y)
{
  m_hasMouse = true;
  damage();
}

//--------------------------------------------------------------
// mouse exited
void mgUglyField::mouseExit(
  void* source)
{
  m_hasMouse = false;
  damage();
}

//--------------------------------------------------------------
// mouse pressed
void mgUglyField::mouseDown(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
  takeKeyFocus();
  damage();
}

//--------------------------------------------------------------
// mouse released
void mgUglyField::mouseUp(
  void* source,
  int x,
  int y,
  int modifiers,
  int button)
{
}

//--------------------------------------------------------------
// mouse clicked
void mgUglyField::mouseClick(
  void* source,
  int x,
  int y,
  int modifiers,
  int button,
  int clickCount)
{
}

//--------------------------------------------------------------
// mouse dragged
void mgUglyField::mouseDrag(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

//--------------------------------------------------------------
// mouse moved
void mgUglyField::mouseMove(
  void* source,
  int x,
  int y,
  int modifiers)
{
}

//--------------------------------------------------------------
void mgUglyField::guiFocusGained(
  void* source)
{
  damage();
}
    
//--------------------------------------------------------------
void mgUglyField::guiFocusLost(
  void* source)
{
  damage();
}
