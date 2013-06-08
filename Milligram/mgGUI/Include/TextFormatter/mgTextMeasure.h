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
#ifndef MGTEXTMEASURE_H
#define MGTEXTMEASURE_H

#include "TextFormatter/mgTextPage.h"
#include "TextFormatter/mgTextScan.h"

/*
  Measures the maximum and minimum widths of an mgTextBuffer when 
  formatted.
*/
class mgTextMeasure : public mgTextScan
{
public: 
  int m_minWidth;                // min width of page
  int m_maxWidth;                // max width of page

  // constructor
  mgTextMeasure(
    mgTextBuffer *buffer,        // text to measure
    mgTextPage* page);           // formatting target

  // destructor
  virtual ~mgTextMeasure();

  // margins have changed
  virtual void newMargins();

  // set a new font
  virtual void newFont();

  // add a space to the line
  virtual void addSpace(
    int value);                  // space width

  // add a tab to the line
  virtual void addTab(
    int value);                  // tab x position

  // end previous word
  virtual void newWord();

  // add a word to the line
  virtual void addFrag(
    BOOL blank,                  // starts with a blank
    const char* text,            // text of string
    int len);                    // length of text

  // add a child box
  virtual void addChild(
    const void* child,
    mgTextAlign halign,
    mgTextAlign valign);         // child added

  // end line, add white space
  virtual void newLine(
    int height);                 // extra vertical space

  // record target position
  virtual void addTarget(
    int *pnY)                     // addr of position
  {} 

  // clear the margins
  virtual void clearMargins(
    mgTextAlign clear)
  {} 

  // end of document
  virtual void done();

protected:
  const void* m_font;             // current font from page

  int m_lineMinWidth;             // min width of line
  int m_lineMaxWidth;             // max width of line
  int m_floatMinWidth;  
  int m_floatMaxWidth;  
  int m_whiteMax;                 // max whitespace seen

  int m_wordWidth;                // size of current word
  int m_wordBlank;                // width of blank in front of word
  BOOL m_afterTab;                // after a tab command

  int m_blankWidth;
};


#endif
