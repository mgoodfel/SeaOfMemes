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

#include "TextFormatter/mgTextScan.h"
#include "TextFormatter/mgTextPage.h"

//-----------------------------------------------------------------
// constructor
mgTextScan::mgTextScan(
  mgTextBuffer* buffer,
  mgTextPage* page)            // formatting target
{
  m_buffer = buffer;
  m_page = page;

  // initialize formatting parms to default
  m_justify = mgTextAlignLeft;
  m_fontFace = "default";
  m_fontSize = 12;
  m_fontItalic = false;
  m_fontBold = false;

  m_leftMargin = 0;
  m_rightMargin = 0;
  m_indent = 0;
  m_color = 0;  // black
//  m_anchor = NULL;
  m_wrap = true;
}

//-----------------------------------------------------------------
// destructor
mgTextScan::~mgTextScan()
{
}

//-----------------------------------------------------------------
// format the textbox contents. 
void mgTextScan::scan(
  unsigned int &posn)            // starting position
{
  // parse buffer and format text
  m_clipped = false;  

  // set state to default
  m_page->getDefaultFormat(m_justify, m_leftMargin, m_rightMargin, m_indent, m_wrap);
  m_page->getDefaultFont(m_fontFace, m_fontSize, m_fontItalic, m_fontBold, m_color);
  m_baseFontSize = m_fontSize;
  newFont();

  // temporaries for when we return values to defaults
  mgString face;
  short left, right, indent, size;
  BOOL wrap, italic, bold;
  DWORD color;
  mgTextAlign justify;

  unsigned int count = 0;
  BOOL endBuffer = false;
  while (!m_clipped && !endBuffer)
  {
    mgFormatCmd cmd = m_buffer->readCommand(posn);
    switch (cmd)
    {
      case mgJustifyCmd: 
      {
        m_justify = m_buffer->readJustify(posn);
        if (m_justify == mgTextAlignHDefault)
          m_page->getDefaultFormat(m_justify, left, right, indent, wrap);
        break;
      }
        
      case mgLeftMarginCmd: 
      {
        short value = m_buffer->readLeftMargin(posn);
        if (value == MGDEFAULTSHORT)
          m_page->getDefaultFormat(justify, m_leftMargin, right, indent, wrap);
        else m_leftMargin = (m_page->getUnits()*value)/100;
        newMargins();
        break;
      }

      case mgRightMarginCmd: 
      {
        short value = m_buffer->readRightMargin(posn);
        if (value == MGDEFAULTSHORT)
          m_page->getDefaultFormat(justify, left, m_rightMargin, indent, wrap);
        else m_rightMargin = (m_page->getUnits()*value)/100;
        newMargins();
        break;
      }

      case mgIndentCmd:
      {
        short value = m_buffer->readIndent(posn);
        if (value == MGDEFAULTSHORT)
          m_page->getDefaultFormat(justify, left, right, m_indent, wrap);
        else m_indent = (m_page->getUnits() * value)/100;
        newMargins();
        break;
      }

      case mgWrapCmd: 
      {
        mgBooleanAttr value = m_buffer->readWrap(posn);
        if (value == mgDefaultBoolean)
          m_page->getDefaultFormat(justify, left, right, indent, m_wrap);
        else m_wrap = value == mgTrue;
        break;
      }

      case mgFontFaceCmd: 
        m_buffer->readFontFace(posn, m_fontFace);
        if (m_fontFace.equals("default"))
          m_page->getDefaultFont(m_fontFace, size, italic, bold, color);
        newFont();
        break;
  
      case mgFontSizeCmd: 
      {
        short value = m_buffer->readFontSize(posn);
        if (value == MGDEFAULTSHORT)
          m_page->getDefaultFont(face, m_fontSize, italic, bold, color);
        else m_fontSize = (m_baseFontSize * value)/100;
        newFont();
        break;
      }
  
      case mgFontItalicCmd: 
      {
        mgBooleanAttr value = m_buffer->readFontItalic(posn);
        if (value == mgDefaultBoolean)
          m_page->getDefaultFont(face, size, m_fontItalic, bold, color);
        else m_fontItalic = value == mgTrue;
        newFont();
        break;
      }
  
      case mgFontBoldCmd: 
      {
        mgBooleanAttr value = m_buffer->readFontBold(posn);
        if (value == mgDefaultBoolean)
          m_page->getDefaultFont(face, size, italic, m_fontBold, color);
        else m_fontBold = value == mgTrue;
        newFont();
        break;
      }
  
      case mgColorCmd:
        m_color = m_buffer->readColor(posn);
        if (m_color == MGDEFAULTCOLOR)
          m_page->getDefaultFont(face, size, italic, bold, m_color);
        break;
        
      case mgAnchorCmd:
//        m_anchor = (mgAnchorDesc*) m_buffer->readPtr(posn);
        break;
          
      case mgSpaceCmd: 
        newWord();
        addSpace(m_buffer->readSpace(posn));
        break;

      case mgTabCmd: 
      {
        newWord();
        short value = m_buffer->readTab(posn);
        value = (m_page->getUnits() * value)/100;
        addTab(value);
        break;
      }

      case mgTextCmd:
      {
        int textLen;
        const char* text;
        m_buffer->readText(posn, textLen, text);

        if (!m_wrap)
          addFrag(false, text, textLen);
        else
        {
          // writeText has converted whitespace to blanks and removed
          // duplicate blanks.  However, a single word may be broken into 
          // multiple fragments due to tags within the word.
          // ex: <b>T</b>est
          while (textLen > 0)
          {
            int c = 0xFF & text[0];
            BOOL isBlank = isspace(c) != 0;
            if (isBlank)
            {
              newWord();
              text++;
              textLen--;
            }
            // count characters of next word
            int len = 0;
            while (len < textLen)
            {
              c = 0xFF & text[len];
              if (!isspace(c))
                len++;
              else break;
            }

            addFrag(isBlank, text, len);

            text += len;
            textLen -= len;
          }
        }
        break;
      }

      case mgChildCmd:
      {
        // read child info 
        const void* child;
        mgTextAlign halign;
        mgTextAlign valign;
        m_buffer->readChild(posn, child, halign, valign);

        newWord();  // breaks the current word
        addChild(child, halign, valign);
        break;
      } 

      case mgBreakCmd: 
      {
        int height = m_buffer->readBreak(posn);
        newWord();  // breaks the current word
        newLine(height);
        break;
      }

      case mgClearCmd: 
      {
        mgTextAlign clear = m_buffer->readClear(posn);
        clearMargins(clear);
        break;
      }

      case mgTargetCmd:
        addTarget((int*) m_buffer->readTarget(posn));
        break;

      case mgDoneCmd:
        newWord();  // breaks the current word
        done();   // end of input
        endBuffer = true;
        break;
    } 
  } 
}

