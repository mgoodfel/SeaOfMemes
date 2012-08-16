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
#include "UglyStyle/mgUglyConsole.h"
#include "UglyStyle/mgUglyStyle.h"

class mgUglyLine
{
public:
  mgColor m_color;
  const mgFont* m_font;
  mgString m_text;
  BOOL m_indent;
};

//--------------------------------------------------------------
// constructor
mgUglyConsole::mgUglyConsole(
  mgControl* parent,
  const char* cntlName)
: mgConsoleControl(parent, cntlName)
{
  // default preferred height of console
  m_displayLines = 10;
  m_historyLines = 500;
  m_lines = new mgPtrArray();

  mgStyle* style = getStyle();

  // assume style has set all our format elements
  style->getFontAttr(MG_STYLE_CONSOLE, m_cntlName, "font", m_font);

  style->getFrameAttr(MG_STYLE_CONSOLE, m_cntlName, "frame", m_frame);
  style->getColorAttr(MG_STYLE_CONSOLE, m_cntlName, "textColor", m_textColor);

  // create the scrollbars, so we can get preferred size correct
  m_vertScroller = new mgUglyScrollbar(this);
  m_vertScroller->addScrollListener(this);

  m_horzScroller = NULL;

  mgDimension vertSize, horzSize;
  if (m_vertScroller != NULL)
    m_vertScroller->preferredSize(vertSize);
  if (m_horzScroller != NULL)
    m_horzScroller->preferredSize(horzSize);

  // save width of vertical scrollbar, height of horizontal scrollbar
  m_scrollSize = mgDimension(vertSize.m_width, horzSize.m_height);

  m_scrollPosn = 0;
  m_scrollVis = 0;
}

//--------------------------------------------------------------
// destructor
mgUglyConsole::~mgUglyConsole()
{
  for (int i = 0; i < m_lines->length(); i++)
  {
    mgUglyLine* msg = (mgUglyLine*) m_lines->getAt(i);
    delete msg;
  }
  m_lines->removeAll();

  delete m_lines;
  m_lines = NULL;
}

//--------------------------------------------------------------
// format a line
void mgUglyConsole::formatLine(
  int formatWidth,
  const mgColor& color,
  const mgFont* font,
  const char* text,
  int textLen)
{
  if (font == NULL)
    font = m_font;

  int blankWidth = font->stringWidth(" ", 1);

  BOOL indent = false;
  int posn = 0;
  while (posn < textLen)
  {
    // indent the continuation lines two blanks
    int lineWidth = formatWidth;
    if (posn != 0)
      lineWidth -= 2*blankWidth;

    // get number of characters which fit on line
    int fitCount = font->stringFit(posn + text, textLen-posn, lineWidth);
    // must put at least 2 characters on a line, but not longer than text
    fitCount = min(textLen-posn, max(2, fitCount));

    // if it does not all fit
    if (posn + fitCount < textLen)
    {
      // back up to previous blank
      int blankPosn = posn+fitCount;
      while (blankPosn > posn)
      {
        if (text[blankPosn] == ' ')
          break;
        blankPosn--;
      }
      // if found a blank, break.  otherwise, use rest of line
      if (blankPosn > posn)
        fitCount = 1+blankPosn-posn;
    }

    // add a line to the list
    mgUglyLine* line = new mgUglyLine();
    line->m_color = color;
    line->m_font = font;
    line->m_text.write(text+posn, fitCount);
    line->m_indent = indent;
    m_lines->add(line);

    indent = true;
    posn += fitCount;
  }
}

//--------------------------------------------------------------
// add a message
void mgUglyConsole::addLine(
  const mgColor& color,
  const mgFont* font,
  const char* message)
{
  // get control width
  mgDimension size;
  getSize(size);

  // subtract scrollbars
  size.m_width -= m_scrollSize.m_width;
  size.m_height -= m_scrollSize.m_height;

  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (m_frame != NULL)
    m_frame->getInsideRect(inside);

  int formatWidth = inside.m_width;
  if (formatWidth <= 0)
    formatWidth = INT_MAX;  // too early.  don't break lines

  // break on newlines
  mgString line;
  for (int i = 0; ; i++)
  {
    char c = message[i];
    if (c == '\0')
      break;
    if (c == '\n')
    {
      formatLine(formatWidth, color, font, line, line.length());
      line.empty();
    }
    else if (c != '\r')
      line += c;
  }
  if (line.length() > 0)
    formatLine(formatWidth, color, font, line, line.length());

  // drop lines over history
  while (m_lines->length() > m_historyLines)
  {
    mgUglyLine* line = (mgUglyLine*) m_lines->getAt(0);
    m_lines->removeAt(0);
    delete line;
  }
    
  m_scrollPosn = 0;  // back to showing end
  updateScrollers();
  damage();
}

//--------------------------------------------------------------
// reformat the lines after resize
void mgUglyConsole::reformatLines(
  int formatWidth)
{
//double start = mgOSGetTime();

  // reformat all the lines
  mgPtrArray* oldLines = m_lines;
  m_lines = new mgPtrArray();
  mgString text;
  mgColor color;
  const mgFont* font = NULL;
  for (int i = 0; i < oldLines->length(); i++)
  {
    mgUglyLine* line = (mgUglyLine*) oldLines->getAt(i);
    // if start of new line, format text
    // check for i==0 because we may have dropped the start of a line
    // due to history max
    if (i == 0 || !line->m_indent)
    {
      if (i != 0)
        formatLine(formatWidth, color, font, text, text.length());

      // start next line
      text = line->m_text;
      color = line->m_color;
      font = line->m_font;
    }
    else text += line->m_text;

    // free old line
    delete line;
  }
  // format the last line, if any
  if (oldLines->length() > 0)
    formatLine(formatWidth, color, font, text, text.length());

  // free old line array
  delete oldLines;
  oldLines = NULL;
//double end = mgOSGetTime();
//mgDebug("reformat time = %g ms", end-start);

  // drop lines over history
  while (m_lines->length() > m_historyLines)
  {
    mgUglyLine* line = (mgUglyLine*) m_lines->getAt(0);
    m_lines->removeAt(0);
    delete line;
  }

  // update scroll position
  m_scrollPosn = min(m_scrollPosn, max(0, m_lines->length() - m_scrollVis)); 
  updateScrollers();
}

//--------------------------------------------------------------
// set max height
void mgUglyConsole::setDisplayLines(
  int lines)
{
  m_displayLines = lines;
  damage();
}

//--------------------------------------------------------------
// set history
void mgUglyConsole::setHistoryLines(
  int lines)
{
  m_historyLines = lines;

  // drop lines over history
  BOOL changed = false;
  while (m_lines->length() > m_historyLines)
  {
    mgUglyLine* line = (mgUglyLine*) m_lines->getAt(0);
    m_lines->removeAt(0);
    delete line;
    changed = true;
  }
  if (changed)
    damage();
}

//--------------------------------------------------------------
// return minimum size
void mgUglyConsole::minimumSize(
  mgDimension& size)
{
  size.m_width = 5*m_font->getAveCharWidth();
  size.m_height = 2*m_font->getHeight();

  // add cntl frame
  if (m_frame != NULL)
  {
    mgDimension outside;
    m_frame->getOutsideSize(size, outside);
    size = outside;
  }

  // add scrollbars
  size.m_width += m_scrollSize.m_width;
  size.m_height += m_scrollSize.m_height;
}  

//--------------------------------------------------------------
// return preferred size
void mgUglyConsole::preferredSize(
  mgDimension& size)
{
  size.m_width = 80* m_font->getAveCharWidth();
  size.m_height = m_displayLines * m_font->getHeight();

  // add cntl frame
  if (m_frame != NULL)
  {
    mgDimension outside;
    m_frame->getOutsideSize(size, outside);
    size = outside;
  }

  // add scrollbars
  size.m_width += m_scrollSize.m_width;
  size.m_height += m_scrollSize.m_height;
}  

//--------------------------------------------------------------
// update layout of controls
void mgUglyConsole::updateLayout()
{
  mgDimension size;
  getSize(size);

  if (m_vertScroller != NULL)
  {
    m_vertScroller->setLocation(size.m_width-m_scrollSize.m_width, 0);
    m_vertScroller->setSize(m_scrollSize.m_width, size.m_height-m_scrollSize.m_height);
    size.m_width -= m_scrollSize.m_width;
  }
  
  if (m_horzScroller != NULL)
  {
    m_horzScroller->setLocation(0, size.m_height-m_scrollSize.m_height);
    m_horzScroller->setSize(size.m_width-m_scrollSize.m_width, m_scrollSize.m_height);
    size.m_height -= m_scrollSize.m_height;
  }

  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (m_frame != NULL)
    m_frame->getInsideRect(inside);
  m_scrollVis = inside.m_height / m_font->getHeight();

  // reformat the text
  int formatWidth = inside.m_width;
  if (formatWidth <= 0)
    formatWidth = INT_MAX;  // too early?  don't break lines
  reformatLines(formatWidth);

  updateScrollers();
}

//--------------------------------------------------------------
// paint content of control
void mgUglyConsole::paint(
  mgContext* gc)
{
  mgDimension size;
  getSize(size);

  // subtract scrollbars
  size.m_width -= m_scrollSize.m_width;
  size.m_height -= m_scrollSize.m_height;

  if (size.m_width <= 0 || size.m_height <= 0)
    return;  // nothing to do

  mgRectangle inside(0, 0, size.m_width, size.m_height);
  if (m_frame != NULL)
  {
    gc->setAlpha(225);
    m_frame->paintBackground(gc, 0, 0, size.m_width, size.m_height);
    m_frame->getInsideRect(inside);
  }

  gc->setAlpha(255);
  gc->setDrawMode(MG_DRAW_RGB);

  // start at the bottom, draw each message.
  int y = inside.m_height;

//double start = mgOSGetTime();

  // empty console means lastline=-1 and nothing is painted
  int lastLine = m_lines->length()-1;
  lastLine -= min(m_lines->length(), m_scrollPosn);
  int charCount = 0;
  BOOL withinClip = true;
  for (int i = lastLine; i >= 0 && withinClip; i--)
  {
    mgUglyLine* line = (mgUglyLine*) m_lines->getAt(i);
    const mgFont* font = line->m_font;
    if (font == NULL)
      font = m_font;

    int descent = font->getHeight() - font->getAscent();
    int blankWidth = font->stringWidth(" ", 1);
    int indent = line->m_indent ? blankWidth*2 : 0;

    // draw the text
    gc->setTextColor(line->m_color);
    gc->setFont(font);
    gc->drawString(line->m_text, line->m_text.length(), inside.m_x+indent, inside.m_y+y-descent);

    charCount += line->m_text.length();
    y -= font->getHeight();
    if (y < inside.m_y)
      withinClip = false;
  }
//double end = mgOSGetTime();
//mgDebug("console redraw text = %g ms, %d chars, %g ms/char", end-start, charCount, (end-start)/charCount);

  if (m_frame != NULL)
    m_frame->paintForeground(gc, 0, 0, size.m_width, size.m_height);
}

//--------------------------------------------------------------
// update scrollbars
void mgUglyConsole::updateScrollers()
{
  // figure new range
  if (m_vertScroller != NULL)
  {
    m_vertScroller->setRange(0, m_lines->length());
    m_vertScroller->setView(m_lines->length() - (m_scrollPosn+m_scrollVis), m_scrollVis);
  }
}

//--------------------------------------------------------------
// line up
void mgUglyConsole::guiScrollLineUp(
  void* source)
{
  if (source == (mgScroller*) m_vertScroller)
  {
    // scrollposn is 0 for bottom, so increment on line up
    int maxPosn = max(0, m_lines->length() - m_scrollVis);
    m_scrollPosn = min(maxPosn, m_scrollPosn+1);
    updateScrollers();
  }
  damage();
}

//--------------------------------------------------------------
// line down
void mgUglyConsole::guiScrollLineDown(
  void* source)
{
  if (source == (mgScroller*) m_vertScroller)
  {
    // scrollposn is 0 for bottom, so decrement on line down
    m_scrollPosn = max(0, m_scrollPosn-1);
    updateScrollers();
  }
  damage();
}

//--------------------------------------------------------------
// page up
void mgUglyConsole::guiScrollPageUp(
  void* source)
{
  if (source == (mgScroller*) m_vertScroller)
  {
    // scrollposn is 0 for bottom, so increment on page up
    int maxPosn = max(0, m_lines->length() - m_scrollVis);
    m_scrollPosn = min(maxPosn, m_scrollPosn+m_scrollVis);
    updateScrollers();
  }
  damage();
}

//--------------------------------------------------------------
// page down
void mgUglyConsole::guiScrollPageDown(
  void* source)
{
  if (source == (mgScroller*) m_vertScroller)
  {
    // scrollposn is 0 for bottom, so decrement on page down
    m_scrollPosn = max(0, m_scrollPosn-m_scrollVis);
    updateScrollers();
  }
  damage();
}

//--------------------------------------------------------------
// set position
void mgUglyConsole::guiScrollSetPosition(
  void* source,
  int value)
{
  if (source == (mgScroller*) m_vertScroller)
  {
    int maxPosn = max(0, m_lines->length() - m_scrollVis);
    m_scrollPosn = maxPosn - min(maxPosn, max(0, value));
    updateScrollers();
  }
  damage();
}
