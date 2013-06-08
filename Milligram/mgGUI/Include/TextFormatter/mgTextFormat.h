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
#ifndef MGTEXTFORMAT_H
#define MGTEXTFORMAT_H

#include "TextFormatter/mgTextPage.h"
#include "TextFormatter/mgTextScan.h"

/*
  A formatted child control in a text pane.
*/
class mgChildDesc
{
public:
  mgChildDesc* m_next;              // next free list entry

  const void* m_child;              // child object

  // position and size
  int m_x;                     
  int m_y;
  int m_width;                 
  int m_height;

  // alignment on page
  mgTextAlign m_horzAlign;
  mgTextAlign m_vertAlign;

  // position in line
  int m_lineX;

  // constructor
  mgChildDesc()
  {}

  // destructor
  virtual ~mgChildDesc()
  {}
};

/*
  A fragment of formatted text.
*/
class mgFragDesc
{
public:
  mgFragDesc* m_next;               // next free list entry

  int m_x;                          // position in line
  const void* m_font;               // font to use
  long m_color;                     // color to use
  //mgAnchorDesc* m_anchor;           // anchor data
  int m_width;                      // width of word
  int m_len;                        // length of string
  const char* m_text;               // pointer to string

  // constructor
  mgFragDesc()
  {}

  // destructor
  virtual ~mgFragDesc()
  {}
};

/*
  Formats an mgTextBuffer into an mgTextPage instance.
*/
class mgTextFormat : public mgTextScan
{
public:
  int m_boxWidth;                // max width of box contents
  int m_boxHeight;               // bottom of box contents
  int m_lineHeight;         // =-= should be protected.  hack to keep demos working.  See mgFormPane minimumSize

  // constructor
  mgTextFormat(
    mgTextBuffer* buffer,        // text to format
    mgTextPage* page,            // formatting target
    BOOL top,                    // true if top box
    int originX,                 // origin of output
    int originY, 
    int pageWidth);              // target page width      

  // destructor
  virtual ~mgTextFormat();

  // get minimum height
  virtual int getMinHeight();

  // margins have changed
  virtual void newMargins();

  // set a new font
  virtual void newFont();

  // add a space to the line
  virtual void addSpace(
    int value);                   // width of space

  // add a tab to the line
  virtual void addTab(
    int value);                   // tab x position

  // end previous word
  virtual void newWord();

  // add a word to the line
  virtual void addFrag(
    BOOL blank,                   // starts with a blank
    const char* text,             // text of string
    int len);                     // length of text

  // add a child box
  virtual void addChild(
    const void* child,
    mgTextAlign halign,
    mgTextAlign valign);         // child added

  // record target position
  virtual void addTarget(
    int* pnY);                    // addr of position

  // end line, add white space
  virtual void newLine(
    int height);                  // extra vertical space

  // clear the margins
  virtual void clearMargins(
    mgTextAlign clear);

  // end of document
  virtual void done();

  // initialize the size of a child box 
  virtual void initChild(
    mgChildDesc* childBox,              // child added 
    int maxWidth);                // max width child can have

  // position child
  virtual void outputChild(
    mgChildDesc* childBox);             // child to move

  // position a word
  virtual void outputLine(
    int left,                     // x position 
    int top,                      // y position
    int height,                   // height of line
    int baseline,                 // baseline of text
    mgPtrArray& frags);           // of mgFragDesc*

protected:
  const void* m_font;             // font from page

  BOOL m_top;                     // true if top box
  int m_originX;                  // output origin
  int m_originY;

  int m_blankWidth;               // size of text in current font
  int m_textHeight;
  int m_textAscent; 

  int m_pageWidth;                // width of page
  int m_lineWidth;                // size of current line
  int m_lineAscent;
  int m_afterTab;                 // true if after tab

  int m_wordWidth;                // size of current word
  int m_wordHeight;
  int m_wordAscent;
  int m_wordBlank;                // width of blank in front of word

  int m_breakHeight;              // break requested 

  // scanner delivers words, since we should remove extra white space.
  mgPtrArray m_lineFrags;
  mgPtrArray m_wordFrags;

  // children are placed in NewLine.  Record child parameters
  mgPtrArray m_inlineChildren;

  // floaters are placed on left and right margins.  Keep a stack
  // of active floats.  .x is new left or right margin, 
  // .y is ending y of child.
  mgPtrArray m_leftFloats;
  mgPtrArray m_rightFloats;

  int m_lineMaxWidth;            // current usable line width

  mgFragDesc* m_freeFrag;         // free list
  mgChildDesc* m_freeChild;       // free list
  
  // create new fragment
  mgFragDesc* newFrag();

  // free fragment
  void freeFrag(
    mgFragDesc* frag);

  // create new child
  mgChildDesc* newChild();

  // free child
  void freeChild(
    mgChildDesc* child);

  // remove floating images from margin stacks
  void popFloaters();

  // find space for new floater
  void findFloatSpace(
    int childWidth);
};

#endif

