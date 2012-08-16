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

#include "TextFormatter/mgTextPage.h"
#include "TextFormatter/mgTextMeasure.h"

//-----------------------------------------------------------------
// constructor
mgTextMeasure::mgTextMeasure(
  mgTextBuffer* buffer,
  mgTextPage* page)
: mgTextScan(buffer, page)
{
  m_lineMinWidth = 0;
  m_lineMaxWidth = 0;
  m_wordWidth = 0;
  m_wordBlank = 0;
  m_floatMinWidth = 0;  
  m_floatMaxWidth = 0;  
  m_whiteMax = 0;

  m_minWidth = 0;
  m_maxWidth = 0;
  m_afterTab = false;
}

//-----------------------------------------------------------------
// destructor
mgTextMeasure::~mgTextMeasure()
{
}

//-----------------------------------------------------------------
// set a new font
void mgTextMeasure::newFont()
{
  m_font = m_page->getFont(m_fontFace, m_fontSize, m_fontItalic, m_fontBold);

  // get size of blank (when formatted with a word -- otherwise get diff value)
  mgPoint size;
  int height;
  int ascent;
  m_page->getFontInfo(m_font, height, ascent, m_blankWidth);
}

//-----------------------------------------------------------------
// add a space to the line
void mgTextMeasure::addSpace(
  int value)                   // space width
{
  m_lineMaxWidth += (value * m_blankWidth)/100;
}

//-----------------------------------------------------------------
// add a tab to the line
void mgTextMeasure::addTab(
  int value)                   // tab x position
{
  // advance to tab position
  m_lineMinWidth = max(m_lineMinWidth, value);
  m_lineMaxWidth = max(m_lineMaxWidth, value);
  m_afterTab = true;
}

//-----------------------------------------------------------------
// start a new word
void mgTextMeasure::newWord()
{
  if (m_wordWidth == 0)
  {
    m_wordBlank = 0;
    return;
  }

  // if at start of line, toss leading blanks
  if (m_lineMaxWidth == 0 || m_afterTab)
    m_wordBlank = 0;

  // start line with indent
  if (m_lineMaxWidth == 0)
    m_lineMaxWidth = m_indent;

  int fitWidth = m_wordBlank + m_wordWidth;

  // add word fragments to max line, as if never a line end
  m_lineMaxWidth += m_wordBlank + m_wordWidth;

  // put word on min line, as if never fits
  m_lineMinWidth = max(m_lineMinWidth, m_indent + m_wordWidth);

  m_wordWidth = 0;
  m_wordBlank = 0;
}

//-----------------------------------------------------------------
// add characters to the line
void mgTextMeasure::addFrag(
  BOOL isBlank,                  // starts with blank
  const char* text,          // text of string
  int len)                     // length of word
{
  if (isBlank)  // there is at most 1 blank per word
    m_wordBlank = m_blankWidth;

  m_wordWidth += m_page->measureString(m_font, text, len);
}

//-----------------------------------------------------------------
// add a child box
void mgTextMeasure::addChild(
  const void* child,
  mgTextAlign halign,
  mgTextAlign valign)  
{
  int boxMinWidth;
  int boxMaxWidth;
  m_page->childWidthRange(child, boxMinWidth, boxMaxWidth);

  switch (halign)
  {
    case mgTextAlignHCenter:
    case mgTextAlignHFill:
    case mgTextAlignInline:
      m_lineMinWidth = max(m_lineMinWidth, boxMinWidth);
      m_lineMaxWidth += boxMaxWidth;
      break;
    
    case mgTextAlignLeft:
    case mgTextAlignRight:
      m_floatMinWidth = max(m_floatMinWidth, boxMinWidth);
      m_floatMaxWidth = max(m_floatMaxWidth, boxMaxWidth);
      break;
  }
}

//-----------------------------------------------------------------
// new margins
void mgTextMeasure::newMargins()
{
  m_whiteMax = max(m_whiteMax, m_leftMargin+m_rightMargin+m_indent);
}

//-----------------------------------------------------------------
// end line, add white space
void mgTextMeasure::newLine(
  int height)                  // extra vertical space
{
  // max line would be text plus float plus margins
  m_maxWidth = max(m_maxWidth, 
                    m_whiteMax + m_lineMaxWidth + m_floatMaxWidth);

  // min line could be either text or float, plus margins
  m_minWidth = max(m_minWidth, m_whiteMax + m_lineMinWidth);
  m_minWidth = max(m_minWidth, m_whiteMax + m_floatMinWidth);

  m_lineMinWidth = m_lineMaxWidth = m_indent;
  m_floatMinWidth = m_floatMaxWidth = 0;
  m_whiteMax = 0;
  newMargins();
}

//-----------------------------------------------------------------
// end of document
void mgTextMeasure::done()
{
  // end last line
  newLine(0);
}
