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
#ifndef MGTEXTPAGE_H
#define MGTEXTPAGE_H

#include "../TextFormatter/mgTextBuffer.h"

// abstract interface to whatever is needed to render text formatter output
class mgTextPage
{
public:
  DWORD m_backRGB;                // background color
  DWORD m_textRGB;                // foreground color
  
  // get descent distance of link underline
  virtual int getLinkDescent() = 0;
  
  // get default font
  virtual void getDefaultFont(
    mgString& face,
    short& size,
    BOOL& italic,
    BOOL& bold, 
    DWORD& color) = 0;

  // get default margins, indent, justify and wrap
  virtual void getDefaultFormat(
    mgTextAlign& justify,
    short& leftMargin,
    short& rightMargin,
    short& indent,
    BOOL& wrap) = 0;

  // get units for sizing on page.  return height of a line in pixels
  virtual int getUnits() = 0;

  // get font
  virtual const void* getFont(
    const char* face,             // face name of font
    short size,                   // size in points
    BOOL italic,                  // true if italic
    BOOL bold) = 0;               // true if bold

  // get font information
  virtual void getFontInfo(
    const void* font,             // returned font
    int &height,                  // line height
    int &ascent,                  // text ascent
    int &blankWidth) = 0;         // width of a blank

  // measure string, return width
  virtual int measureString(
    const void* font,             // returned font
    const char* string,           // string to measure
    int len = -1) = 0;            // length of string, -1 for strlen

  // draw string at coordinates
  virtual void drawString(
    const void* font,             // returned font
    long color,                   // color in RGB format
    int x,                        // x coordinate of first char
    int y,                        // y coordinate of baseline
    const char* string,           // string to measure
    int len = -1) = 0;            // length of string, -1 for strlen

  // reset the children of a child
  virtual void childReset(
    const void* child) = 0;

  // get width range of child
  virtual void childWidthRange(
    const void* child,
    int& minWidth,
    int& maxWidth) = 0;

  // get preferred size at width
  virtual void childSizeAtWidth(
    const void* child,
    int width,
    mgDimension& size) = 0;

  // set child position
  virtual void childSetPosition(
    const void* child,
    int x,
    int y,
    int width,
    int height) = 0;
};

#endif
