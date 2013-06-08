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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "TextFormatter/mgTextPage.h"
#include "TextFormatter/mgTextFormat.h"

//-----------------------------------------------------------------
// constructor
mgTextFormat::mgTextFormat(
  mgTextBuffer* buffer,         // text being formatted
  mgTextPage* page,             // page 
  BOOL top,                     // true if this is top-level box
  int originX,                  // origin of box
  int originY, 
  int pageWidth)                // target page width
: mgTextScan(buffer, page)
{
  m_top = top;
  m_originX = originX;
  m_originY = originY;

  m_freeFrag = NULL;
  m_freeChild = NULL;

  // figure maximum line width
  m_pageWidth = pageWidth;
  newMargins();

  // initialize current extents
  m_afterTab = false;
  m_lineWidth = 0;   
  m_lineHeight = 0;
  m_lineAscent = 0;
  m_wordWidth = 0;
  m_wordBlank = 0;
  m_wordHeight = 0;
  m_wordAscent = 0;
  m_boxHeight = 0;  // height of box
  m_boxWidth = 0;  // max width seen

  m_textHeight = 1;
  m_textAscent = 1;
  m_blankWidth = 1;

  m_breakHeight = 0;
}

//-----------------------------------------------------------------
// destructor
mgTextFormat::~mgTextFormat()
{
  // delete any unused word fragments
  for (int i = 0; i < m_wordFrags.length(); i++)
  {
    freeFrag((mgFragDesc*) m_wordFrags[i]);
  }
  m_wordFrags.removeAll();

  // delete unused line fragments
  for (int i = 0; i < m_lineFrags.length(); i++)
  {
    freeFrag((mgFragDesc*) m_lineFrags[i]);
  }
  m_lineFrags.removeAll();

  // delete the fragment free list
  mgFragDesc* frag = m_freeFrag;
  while (frag != NULL)
  {
    mgFragDesc* next = frag->m_next;
    delete frag;
    frag = next;
  }

  // delete child free list
  mgChildDesc* child = m_freeChild;
  while (child != NULL)
  {
    mgChildDesc* next = child->m_next;
    delete child;
    child = next;
  }
}

//-----------------------------------------------------------------
// return minimum height for formatted text -- one line
int mgTextFormat::getMinHeight()
{
  return m_textHeight;
}

//-----------------------------------------------------------------
// set a new font
void mgTextFormat::newFont()
{
  m_font = m_page->getFont(m_fontFace, m_fontSize, m_fontItalic, m_fontBold);
  m_page->getFontInfo(m_font, m_textHeight, m_textAscent, m_blankWidth);
}

//-----------------------------------------------------------------
// add a space to the line
void mgTextFormat::addSpace(
  int value)                   // width of space
{
  if (m_wrap && !m_afterTab && m_lineWidth != 0)  // no leading blanks
    m_lineWidth += (value * m_blankWidth)/2;
}

//-----------------------------------------------------------------
// add a tab to the line
void mgTextFormat::addTab(
  int value)                   // tab x position
{
  // advance to tab position
  if (m_lineWidth < value)
  {
    m_lineWidth = value;
    m_afterTab = true;
  }
}

//-----------------------------------------------------------------
// margins or floats have changed, compute new max line width
void mgTextFormat::newMargins()
{
  // line ends at right floaters, if any
  if (m_rightFloats.length() > 0)
  {
    mgChildDesc* childBox = (mgChildDesc*) m_rightFloats.last();
    m_lineMaxWidth = childBox->m_x;
  }
  else m_lineMaxWidth = m_pageWidth;

  // subtract left floats
  if (m_leftFloats.length() > 0)
  {
    mgChildDesc* childBox = (mgChildDesc*) m_leftFloats.last();
    m_lineMaxWidth -= childBox->m_x + childBox->m_width;
  }

  // subtract margins
  m_lineMaxWidth -= m_leftMargin + m_rightMargin;
}

//-----------------------------------------------------------------
// remove floating images from margin stacks
void mgTextFormat::popFloaters()
{
  // pop any left side floaters which have expired
  while (m_leftFloats.length() > 0)
  {
    mgChildDesc* childBox = (mgChildDesc*) m_leftFloats.last();
    if (childBox->m_y + childBox->m_height <= m_boxHeight)
    {
      m_leftFloats.pop();
      freeChild(childBox);
    }
    else break;
  }

  // pop any right side floaters which have expired
  while (m_rightFloats.length() > 0)
  {
    mgChildDesc* childBox = (mgChildDesc*) m_rightFloats.last();
    if (childBox->m_y + childBox->m_height <= m_boxHeight)
    {
      m_rightFloats.pop();
      freeChild(childBox);
    }
    else break;
  }

  newMargins();
}

//-----------------------------------------------------------------
// find space for new floater
void mgTextFormat::findFloatSpace(
  int childWidth)
{
  while (m_lineMaxWidth < childWidth)
  {
    int nextHeight = INT_MAX;
    if (m_leftFloats.length() > 0)
    {
      mgChildDesc* childBox = (mgChildDesc*) m_leftFloats.last();
      nextHeight = min(nextHeight, childBox->m_y + childBox->m_height);
    }
    if (m_rightFloats.length() > 0)
    {
      mgChildDesc* childBox = (mgChildDesc*) m_rightFloats.last();
      nextHeight = min(nextHeight, childBox->m_y + childBox->m_height);
    }
    if (nextHeight != INT_MAX)
    {
      m_boxHeight = nextHeight;
      popFloaters();
    }
    else break;   // no floaters to pop
  }
}

//-----------------------------------------------------------------
// start a new word
void mgTextFormat::newWord()
{
  if (m_wordWidth == 0)
  {
    m_wordBlank = 0;
    return;
  }

  // if at start of line, toss leading blanks
  if (m_lineWidth == 0 || m_afterTab)
    m_wordBlank = 0;

  // start line with indent
  if (m_lineWidth == 0)
    m_lineWidth = m_indent;

  // see if word fits
  int fitWidth = m_wordBlank + m_wordWidth;
  if (m_lineWidth > 0 && m_lineWidth + fitWidth > m_lineMaxWidth)
  {
    newLine(0);
    m_lineWidth = m_indent;
    fitWidth = m_wordWidth;  // start of line, toss leading blanks
    m_wordBlank = 0;
  }

  // if margins too narrow due to floaters, pop them
  if (m_lineMaxWidth < fitWidth)
    findFloatSpace(fitWidth);

  // add word fragments to current line
  m_lineWidth += m_wordBlank;

  for (int i = 0; i < m_wordFrags.length(); i++)
  {
    mgFragDesc* frag = (mgFragDesc*) m_wordFrags[i];

    // can we merge this word fragment with last line fragment?
    if (m_lineFrags.length() > 0)
    {
      mgFragDesc* prev = (mgFragDesc*) m_lineFrags.last();
      const char* endText = prev->m_text + prev->m_len;
      if (m_wordBlank > 0)
        endText++;

      if (endText == frag->m_text &&
          prev->m_font == frag->m_font &&
          prev->m_color == frag->m_color)// &&
//          prev->m_anchor == frag->m_anchor)
      {
        // add new word frag to last line frag
        prev->m_len += frag->m_len;
        prev->m_width += frag->m_width;
        if (m_wordBlank > 0)
        {
          prev->m_len++;
          prev->m_width += m_wordBlank;
        }
        m_lineWidth += frag->m_width;

        freeFrag(frag);
        continue;
      }

      m_wordBlank = 0;
    }

    // add new word fragment to line
    frag->m_x = m_lineWidth;
    m_lineFrags.add(frag);
    m_lineWidth += frag->m_width;
  }

  m_wordFrags.removeAll();
  m_afterTab = false;

  m_lineHeight = max(m_lineHeight, m_wordHeight);
  m_lineAscent = max(m_lineAscent, m_wordAscent);

  m_wordWidth = 0;
  m_wordBlank = 0;
  m_wordHeight = 0;
  m_wordAscent = 0;
}

//-----------------------------------------------------------------
// add text to the current word
void mgTextFormat::addFrag(
  BOOL isBlank,                   // starts with a blank
  const char* text,               // text of string
  int len)                        // length of word
{
  if (isBlank)  // there is at most 1 blank per word
    m_wordBlank = m_blankWidth;

  m_wordHeight = max(m_wordHeight, m_textHeight);
  m_wordAscent = max(m_wordAscent, m_textAscent);

  if (len == 0)
    return;

  int width = m_page->measureString(m_font, text, len);
  m_wordWidth += width;

  // save fragment
  mgFragDesc* frag = newFrag();
  frag->m_font = m_font;
  frag->m_color = m_color;
//  frag->m_anchor = m_anchor;
  frag->m_width = width;
  frag->m_len = len;
  frag->m_text = text;

  // add fragment to the word 
  m_wordFrags.add(frag);
}

//-----------------------------------------------------------------
// initialize the size of a child box 
void mgTextFormat::initChild(
  mgChildDesc* childBox,       // child added 
  int maxWidth)                // max width child can have
{
  // if first pass, format child to requested width

  //=-= maxWidth varies when page resized.  It's not equal to pageWidth
  //=-= either, since it depends on margins, figure placements, etc.
  //=-= to avoid doing this on each formatter pass, would need to keep
  //=-= maxWidth from last formatting and check if it changes.
  
  //=-= if (box->m_boxWidth == 0)
  {
#ifdef TRACE_TABLES
mgDebug("initChild %d...", maxWidth);
#endif
    mgDimension boxSize;
    m_page->childSizeAtWidth(childBox->m_child, maxWidth, boxSize);
    childBox->m_width = boxSize.m_width;
    childBox->m_height = boxSize.m_height;
#ifdef TRACE_TABLES
mgDebug("child box is %d by %d", childBox->m_width, childBox->m_height);
#endif
  }
}

//-----------------------------------------------------------------
// add a child box
void mgTextFormat::addChild(
  const void* child,
  mgTextAlign halign,
  mgTextAlign valign)         // child added
{
  int childMinWidth, childMaxWidth;
  m_page->childWidthRange(child, childMinWidth, childMaxWidth);

  mgChildDesc* childBox = newChild();
  childBox->m_child = child;
  childBox->m_horzAlign = halign;
  childBox->m_vertAlign = valign;

  switch (halign)
  {
    case mgTextAlignInline:
    case mgTextAlignHCenter:
    case mgTextAlignHFill:
    {
      // if doesn't fit in current line
      if (m_lineWidth + childMinWidth > m_lineMaxWidth)
        newLine(0);

      // pop floaters if not enough room
      findFloatSpace(childMinWidth);

      m_inlineChildren.add(childBox);

      // set size of child, up to remaining width 
      initChild(childBox, m_lineMaxWidth - m_lineWidth);

      // if fill, set width to remaining width
      if (halign == mgTextAlignHFill)
        childBox->m_width = m_lineMaxWidth - m_lineWidth;

      // set position
      childBox->m_lineX = m_lineWidth;
      m_lineWidth += childBox->m_width;

      switch (childBox->m_vertAlign)
      {
        case mgTextAlignTop:
          // top at top of line
          m_lineHeight = max(m_lineHeight, childBox->m_height);
        break;
        case mgTextAlignBottom:
          // bottom at baseline
          m_lineHeight = max(m_lineHeight, childBox->m_height + (m_lineHeight - m_lineAscent));
          m_lineAscent = max(m_lineAscent, childBox->m_height);
        break;
        case mgTextAlignVCenter:
        case mgTextAlignVFill:
          // centerline on baseline
          m_lineHeight = max(m_lineHeight, childBox->m_height);
          m_lineAscent = max(m_lineAscent, childBox->m_height/2);
        break;
      }
      break;
    }

    case mgTextAlignLeft:
    {
      // get desired size of child
      initChild(childBox, m_lineMaxWidth);

      // if doesn't fit in current line
      if (m_lineWidth + childBox->m_width > m_lineMaxWidth)
        newLine(0);

      // child needs to be added to left margin
      findFloatSpace(childBox->m_width);

      // set position
      childBox->m_x = 0;
      if (m_leftFloats.length() > 0)
      {
        mgChildDesc* leftBox = (mgChildDesc*) m_leftFloats.last();
        childBox->m_x = leftBox->m_x + leftBox->m_width;
      }
      childBox->m_y = m_boxHeight;
      outputChild(childBox);

      m_leftFloats.add(childBox);

      newMargins();
      break;
    }

    case mgTextAlignRight:
    {
      // get desired size of child
      initChild(childBox, m_lineMaxWidth);

      // if doesn't fit in current line
      if (m_lineWidth + childBox->m_width > m_lineMaxWidth)
        newLine(0);

      // child needs to be added to right margin
      findFloatSpace(childBox->m_width);

      // set position
      childBox->m_x = m_pageWidth - childBox->m_width;
      if (m_rightFloats.length() > 0)
      {
        mgChildDesc* rightBox = (mgChildDesc*) m_rightFloats.last();
        childBox->m_x = max(0, rightBox->m_x - childBox->m_width);
      }
      childBox->m_y = m_boxHeight;
      outputChild(childBox);

      m_rightFloats.add(childBox);

      newMargins();
      break;
    }
  }
}

//-----------------------------------------------------------------
// record target position
void mgTextFormat::addTarget(
  int* pnY)                     // addr of position
{
  // record current position
  if (pnY != NULL)
    *pnY = m_originY + m_boxHeight;
} 

//-----------------------------------------------------------------
// clear the margins
void mgTextFormat::clearMargins(
  mgTextAlign clear)
{
#ifdef WORKED
  // pop floaters off left until there are none
  if (clear == mgTextAlignLeft || clear == mgTextAlignHCenter)
  {
    while (m_leftTop > 0)
    {
      m_boxHeight = m_leftFloats[m_leftTop].m_y;
      popFloaters();
    }
  }
  // pop floaters off right until there are none
  if (clear == mgTextAlignRight || clear == mgTextAlignHCenter)
  {
    while (m_rightTop > 0)
    {
      m_boxHeight = m_rightFloats[m_rightTop].m_y;
      popFloaters();
    }
  }
#endif
}

//-----------------------------------------------------------------
// end a line
void mgTextFormat::newLine(
  int height)                  // additional vertical space
{
  // ignore a newline after a tab, to handle <li><p>.  Ick!
  if (m_afterTab)
    return;

  // add previous break height
  if (m_lineHeight != 0)
  {
    m_boxHeight += m_breakHeight;
    m_breakHeight = 0;
  }

  // figure horizontal alignment of line
  int horzAdjust = 0;
  switch (m_justify)
  {
    case mgTextAlignLeft:
      horzAdjust = 0;
      break;

    case mgTextAlignHCenter:
      horzAdjust = (m_lineMaxWidth - m_lineWidth)/2;
      break;

    case mgTextAlignRight:
      horzAdjust = m_lineMaxWidth - m_lineWidth;
      break;
  }
  // Make sure we do not go off the left margin.
  horzAdjust = max(0, horzAdjust);

  // add margin and centering
  int textX = m_leftMargin + horzAdjust;

  // add position of left floater
  if (m_leftFloats.length() > 0)
  {
    mgChildDesc* childBox = (mgChildDesc*) m_leftFloats.last();
    textX += childBox->m_x + childBox->m_width;
  }

  // output words
  outputLine(textX, m_boxHeight, m_lineHeight, m_lineAscent, m_lineFrags);

  // delete used fragments
  for (int i = 0; i < m_lineFrags.length(); i++)
  {
    freeFrag((mgFragDesc*) m_lineFrags[i]);
  }
  m_lineFrags.removeAll();

  m_boxHeight += m_lineHeight;
  m_boxWidth = max(m_boxWidth, m_lineWidth + textX);
      
  popFloaters();

  // position inline children
  for (int i = 0; i < m_inlineChildren.length(); i++)
  {
    mgChildDesc* childBox = (mgChildDesc*) m_inlineChildren[i];

    // do vertical alignment, add line horizontal origin
    int vertAdjust = 0;
    switch (childBox->m_vertAlign)
    {
      case mgTextAlignTop: 
      case mgTextAlignVFill:
        vertAdjust = 0;
        break;
      case mgTextAlignVCenter: 
        vertAdjust = (m_lineHeight - childBox->m_height)/2;
        break;
      case mgTextAlignBottom: 
        vertAdjust = m_lineHeight - childBox->m_height;
        break;
    } 
    // position child
    childBox->m_x = childBox->m_lineX + textX;
    childBox->m_y = m_boxHeight - m_lineHeight + vertAdjust;
    outputChild(childBox);

    freeChild(childBox);
  }

  // inline children used
  m_inlineChildren.removeAll();

  if (m_indent > 0)
    findFloatSpace(m_indent);

  // set next break height 
  if (m_boxHeight > 0)  // nothing before first line
  {
    if (height >= 0)
      m_breakHeight = max(m_breakHeight, (height * m_textHeight)/100);
    else
    { 
      // negative height used by <br>.  <br><br> should produce blank line.  Ick!
      if (m_lineWidth == 0)
        m_breakHeight += (-height * m_textHeight)/100;
    }
  }

  m_afterTab = false;
  m_lineWidth = 0;
  m_lineHeight = 0;
  m_lineAscent = 0;
}

//-----------------------------------------------------------------
// position child
void mgTextFormat::outputChild(
  mgChildDesc* childBox)             // positioned child
{
  // if this is the top box, then this is the final position of the 
  // child, and all its descendants.
//  if (m_top)
  m_page->childSetPosition(childBox->m_child, 
    m_originX + childBox->m_x, m_originY + childBox->m_y,
    childBox->m_width, childBox->m_height);
}

//-----------------------------------------------------------------
// add anchor text to list
void mgTextFormat::outputLine(
  int left,                    // x position 
  int top,                     // y position
  int height,                  // height of line
  int baseline,                // baseline of text
  mgPtrArray& lineFrags)       // text fragments in line
{
  for (int i = 0; i < lineFrags.length(); i++)
  {
    mgFragDesc* frag = (mgFragDesc*) lineFrags[i];
#ifdef WORKED
    if (frag->m_anchor != NULL)
    {
      mgAnchorDesc* anchor = frag->m_anchor;

      mgAnchorRect* rect = new mgAnchorRect;
      rect->m_anchor = anchor;
      // keep bounds in local coordinates
      rect->m_bounds.m_x = left + frag->m_x;
      rect->m_bounds.m_y = top;
      rect->m_bounds.m_width = frag->m_width;
      rect->m_bounds.m_height = height;
      m_box->m_anchors.add(rect);
    }
#endif
  }
}

//-----------------------------------------------------------------
// end of document
void mgTextFormat::done()
{
  // end last line
  newLine(0);

#ifdef WORKED
  // finish off floaters
  while (m_leftTop > 0 || m_rightTop > 0)
  {
    m_boxHeight = min(m_leftFloats[m_leftTop].m_y, m_rightFloats[m_rightTop].m_y);
    popFloaters();
  }
#endif
}

//-----------------------------------------------------------------
// create new fragment
mgFragDesc* mgTextFormat::newFrag()
{
  mgFragDesc* result;
  if (m_freeFrag != NULL)
  {
    result = m_freeFrag;
    m_freeFrag = m_freeFrag->m_next;
  }
  else result = new mgFragDesc();

  result->m_next = NULL;
  result->m_font = NULL;
  result->m_text = NULL;

  return result;
}

//-----------------------------------------------------------------
// free fragment
void mgTextFormat::freeFrag(
  mgFragDesc* frag)
{
  frag->m_next = m_freeFrag;
  m_freeFrag = frag;
}

//-----------------------------------------------------------------
// create new child
mgChildDesc* mgTextFormat::newChild()
{
  mgChildDesc* result;
  if (m_freeChild != NULL)
  {
    result = m_freeChild;
    m_freeChild = m_freeChild->m_next;
  }
  else result = new mgChildDesc();

  result->m_next = NULL;
  result->m_child = NULL;

  return result;
}

//-----------------------------------------------------------------
// free child
void mgTextFormat::freeChild(
  mgChildDesc* child)
{
  child->m_next = m_freeChild;
  m_freeChild = child;
}
