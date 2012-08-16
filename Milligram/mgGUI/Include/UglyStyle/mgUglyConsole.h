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
#ifndef MGUGLYCONSOLE_H
#define MGUGLYCONSOLE_H

#include "GUI/mgScrollListener.h"
#include "GUI/mgConsoleControl.h"

class mgFrame;
class mgUglyScrollbar;
class mgUglyLine;

class mgUglyConsole : public mgConsoleControl, public mgScrollListener
{
public:
  // constructor
  mgUglyConsole(
    mgControl* parent,
    const char* cntlName = NULL);

  // destructor
  virtual ~mgUglyConsole();

  // add a line
  virtual void addLine(
    const mgColor& color,
    const mgFont* font,
    const char* text);

  // set preferred height
  virtual void setDisplayLines(
    int lines);

  // set history
  virtual void setHistoryLines(
    int lines);

  // return minimum size
  virtual void minimumSize(
    mgDimension& size);

  // return preferred size
  virtual void preferredSize(
    mgDimension& size);
    
protected:
  int m_displayLines;
  int m_historyLines;

  mgPtrArray* m_lines;
  int m_scrollVis;
  int m_scrollPosn;

  mgUglyScrollbar* m_vertScroller;
  mgUglyScrollbar* m_horzScroller;
  mgDimension m_scrollSize;

  const mgFont* m_font;
  const mgFrame* m_frame;
  mgColor m_textColor;

  // format a line
  virtual void formatLine(
    int formatWidth,
    const mgColor& color,
    const mgFont* font,
    const char* text,
    int textLen);

  // reformat the lines after resize
  virtual void reformatLines(
    int formatWidth);

  // paint content of control
  virtual void paint(
    mgContext* gc);

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

