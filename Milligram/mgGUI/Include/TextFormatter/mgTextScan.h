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
#ifndef MGTEXTSCAN_H
#define MGTEXTSCAN_H

#include "TextFormatter/mgTextBuffer.h"

class mgTextPage;

class mgTextScan
{
public:
  // constructor
  mgTextScan(
    mgTextBuffer *buffer,
    mgTextPage* page);            // formatting target

  // destructor
  virtual ~mgTextScan();

  // scan the textbox contents, adding words and children.
  void scan(
    unsigned int &posn);         // starting position

  // margins have changed
  virtual void newMargins() = 0;

  // set a new font
  virtual void newFont() = 0;

  // add space to the line
  virtual void addSpace(
    int value) = 0;              // width of space

  // add a tab to the line
  virtual void addTab(
    int value) = 0;              // tab x position

  // end previous word
  virtual void newWord() = 0;

  // add characters to the line
  virtual void addFrag(
    BOOL blank,                       // starts with a blank
    const char* text,                 // text of string
    int len) = 0;                     // length of text

  // add a child box.
  virtual void addChild(
    const void* child,
    mgTextAlign halign,
    mgTextAlign valign) = 0;          // child added

  // record target position
  virtual void addTarget(
    int *pnY) = 0;                    // addr of position

  // end line, add white space
  virtual void newLine(
    int nHeight) = 0;                 // extra vertical space

  // clear the margins
  virtual void clearMargins(
    mgTextAlign clear) = 0;

  // end of document
  virtual void done() = 0;

protected:
  mgTextPage* m_page;                 // page to format against
  mgTextBuffer *m_buffer;             // text to scan
  BOOL m_clipped;                     // true when formatting outside clip

  mgString m_fontFace;
  short m_fontSize;
  BOOL m_fontItalic;
  BOOL m_fontBold;

  short m_baseFontSize;               // buffer sizes multiply default size

  mgTextAlign m_justify;              // justification
  short m_leftMargin;                 // current left margin
  short m_rightMargin;                // current right margin
  short m_indent;                     // indent on next line
  DWORD m_color;                      // current color
  BOOL m_wrap;                        // true to wrap text
//  mgAnchorDesc *m_anchor;             // anchor data
};



#endif

