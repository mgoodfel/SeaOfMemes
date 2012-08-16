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

#ifndef MGFORMPANE_H
#define MGFORMPANE_H

#include "../GUI/mgFormControl.h"
#include "../TextFormatter/mgTextPage.h"

class mgTextBuffer;

class mgFormPane : public mgFormControl, public mgTextPage
{
public:
  mgTextBuffer* m_text;
  
  // constructor
  mgFormPane(
    mgControl* parent,
    const char* cntlName = NULL,
    mgTextBuffer* text = NULL);    // buffer to display (pane owns buffer)

  // destructor
  virtual ~mgFormPane();
  
  // parse XML form definition
  virtual void setForm(
    const char* xml,
    int len = -1);   // len=-1 means strlen(xml)

  // parse XML form file
  virtual void parseFormFile(
    const char* fileName);

  // set default font face
  virtual void setDefaultFontFace(
    const char* faceName);

  // return default font face
  virtual void getDefaultFontFace(
    mgString& faceName);

  // set default font size
  virtual void setDefaultFontSize(
    int size);

  // get default font size
  virtual int getDefaultFontSize();

  // set default text color
  virtual void setDefaultTextColor(
    const mgColor& color);

  // return default text color
  virtual void getDefaultTextColor(
    mgColor& color);

  // set control frame
  virtual void setFrame(
    const mgFrame* frame);

  // get units for sizing on page.  return height of a line in pixels
  virtual int getUnits();

  // return minimum size
  virtual void minimumSize(
    mgDimension& size);

  // return preferred size
  virtual void preferredSize(
    mgDimension& size);
    
  // compute size at width.  return false if not implemented
  virtual BOOL preferredSizeAtWidth(
    int width,
    mgDimension& size);

  // set a control name
  virtual void setCntlName(
    const char* name,
    mgControl* control);

  // get control by name
  virtual mgControl* getCntl(
    const char* cntlName);

protected:
  mgString m_defaultFontFace;
  int m_defaultFontSize;
  mgColor m_defaultTextColor;
  const mgFrame* m_frame;

  mgMapStringToPtr* m_cntlNames;

  mgSurface* m_surface;
  mgContext* m_measureGC;    
  mgContext* m_drawGC;

  // get descent distance of link underline
  virtual int getLinkDescent();
  
  // get default font
  virtual void getDefaultFont(
    mgString& face,               // font face
    short& size,                  // size in points
    BOOL& italic,
    BOOL& bold,
    DWORD& color);
  
  // get default margins, indent, justify and wrap
  virtual void getDefaultFormat(
    mgTextAlign& justify,
    short& leftMargin,            // pixels
    short& rightMargin,           // pixels
    short& indent,                // pixels
    BOOL& wrap);

  // return pixels for points on page
  virtual int points(
    double ptSize);               // points

  // get font
  virtual const void* getFont(
    const char* face,             // face name of font
    short size,                   // size in points
    BOOL italic,                  // true if italic
    BOOL bold);                   // true if bold

  // get font information
  virtual void getFontInfo(
    const void* font,             // returned font
    int &height,                  // line height
    int &ascent,                  // text ascent
    int &blankWidth);             // width of a blank

  // measure string, return width
  virtual int measureString(
    const void* font,             // returned font
    const char* string,           // string to measure
    int len = -1);                // length of string, -1 for strlen

  // draw string at coordinates
  virtual void drawString(
    const void* font,             // returned font
    long color,                   // color in RGB format
    int x,                        // x coordinate of first char
    int y,                        // y coordinate of baseline
    const char* string,           // string to measure
    int len = -1);                // length of string, -1 for strlen

  // reset the children of a child
  virtual void childReset(
    const void* child);

  // get width range of child
  virtual void childWidthRange(
    const void* child,
    int& minWidth,
    int& maxWidth);

  // get preferred size at width
  virtual void childSizeAtWidth(
    const void* child,
    int width,
    mgDimension& size);

  // set child position
  virtual void childSetPosition(
    const void* child,
    int x,
    int y,
    int width,
    int height);

  // paint content of control
  virtual void paint(
    mgContext* gc);
};

#endif
