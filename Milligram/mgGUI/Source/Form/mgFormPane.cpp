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
#include "TextFormatter/mgTextBuffer.h"
#include "TextFormatter/mgTextMeasure.h"
#include "TextFormatter/mgTextFormat.h"
#include "TextFormatter/mgTextDraw.h"

#include "Form/mgFormParser.h"
#include "Form/mgFormPane.h"

//--------------------------------------------------------------
// constructor
mgFormPane::mgFormPane(
  mgControl* parent,
  const char* cntlName,
  mgTextBuffer* text)
: mgFormControl(parent, cntlName)
{
  if (text != NULL)
    m_text = text;
  else m_text = new mgTextBuffer();

  m_cntlNames = NULL;

  m_surface = getSurface();
  m_measureGC = m_surface->newContext();

  m_frame = NULL;
  m_defaultFontFace = "Arial";
  m_defaultFontSize = 10;
  m_defaultTextColor = mgColor("white");
}

//--------------------------------------------------------------
// destructor
mgFormPane::~mgFormPane()
{
  delete m_text;
  m_text = NULL;

  delete m_measureGC;
  m_measureGC = NULL;

  delete m_cntlNames;
  m_cntlNames = NULL;
}

//--------------------------------------------------------------
// set default font face
void mgFormPane::setDefaultFontFace(
  const char* faceName)
{
  m_defaultFontFace = faceName;
  damage();
}

//--------------------------------------------------------------
// get default font face
void mgFormPane::getDefaultFontFace(
  mgString& faceName)
{
  faceName = m_defaultFontFace;
}

//--------------------------------------------------------------
// set default font size
void mgFormPane::setDefaultFontSize(
  int size)
{
  m_defaultFontSize = size;
  damage();
}

//--------------------------------------------------------------
// get default font size
int mgFormPane::getDefaultFontSize()
{
  return m_defaultFontSize;
}

//--------------------------------------------------------------
// set default text color
void mgFormPane::setDefaultTextColor(
  const mgColor& color)
{
  m_defaultTextColor = color;
  damage();
}

//--------------------------------------------------------------
// get default text color
void mgFormPane::getDefaultTextColor(
  mgColor& color)
{
  color = m_defaultTextColor;
}

//--------------------------------------------------------------
// set control frame
void mgFormPane::setFrame(
  const mgFrame* frame)
{
  m_frame = frame;
  damage();
}

//--------------------------------------------------------------
// parse XML form text
void mgFormPane::setForm(
  const char* xml,
  int len)
{
  delete m_text;
  m_text = new mgTextBuffer();

  if (len == -1)
    len = strlen(xml);

  mgFormParser parser(this);
  parser.parse(len, xml);
  parser.parseEnd();

  // take control names table from parser
  m_cntlNames = parser.m_cntlNames;
  parser.m_cntlNames = NULL;

  damage();
}

//--------------------------------------------------------------
// parse XML form file
void mgFormPane::parseFormFile(
  const char* fileName)
{
  delete m_text;
  m_text = new mgTextBuffer();

  mgFormParser parser(this);
  parser.parseFile(fileName);

  // take control names table from parser
  m_cntlNames = parser.m_cntlNames;
  parser.m_cntlNames = NULL;

  damage();
}

//--------------------------------------------------------------
// set a control name
void mgFormPane::setCntlName(
  const char* name,
  mgControl* control)
{
  if (m_cntlNames == NULL)
    m_cntlNames = new mgMapStringToPtr();

  m_cntlNames->setAt(name, control);
}

//--------------------------------------------------------------
// get control by name
mgControl* mgFormPane::getCntl(
  const char* cntlName)
{
  if (m_cntlNames == NULL)
    return NULL;

  const void* value;
  if (!m_cntlNames->lookup(cntlName, value))
    return NULL;

  return (mgControl*) value;
}

//--------------------------------------------------------------
// get descent distance of link underline
int mgFormPane::getLinkDescent()
{
  return 0;
}

//--------------------------------------------------------------
// get default font
void mgFormPane::getDefaultFont(
  mgString& face,
  short& size,
  BOOL& italic,
  BOOL& bold,
  DWORD& color)
{
  face = m_defaultFontFace;
  size = m_defaultFontSize;
  italic = false;
  bold = false;
  color = m_defaultTextColor.asRGB();
}

//--------------------------------------------------------------
// get default margins, indent, justify and wrap
void mgFormPane::getDefaultFormat(
  mgTextAlign& justify,
  short& leftMargin,
  short& rightMargin,
  short& indent,
  BOOL& wrap)
{
  justify = mgTextAlignLeft;
  leftMargin = 0;
  rightMargin = 0;
  indent = 0;
  wrap = true;
}

//--------------------------------------------------------------
// return pixels for points on page
int mgFormPane::points(
  double ptSize)
{
  return m_surface->points(ptSize);
}

//--------------------------------------------------------------
// get units for sizing on page.  return height of a line in pixels
int mgFormPane::getUnits()
{
  return m_surface->points(m_defaultFontSize);
}

//--------------------------------------------------------------
// get font
const void* mgFormPane::getFont(
  const char* face,             // face name of font
  short size,                   // size
  BOOL italic,                  // true if italic
  BOOL bold)               // true if bold
{
  return (const void*) m_surface->createFont(face, size, bold, italic);
}

//--------------------------------------------------------------
// get font information
void mgFormPane::getFontInfo(
  const void* font,             // returned font
  int &height,                  // line height
  int &ascent,                  // text ascent
  int &blankWidth)              // width of a blank
{
  const mgFont* realFont = (const mgFont*) font;
  height = realFont->getHeight();
  ascent = realFont->getAscent();
  blankWidth = realFont->stringWidth(" ", 1);
}

//--------------------------------------------------------------
// measure string, return width
int mgFormPane::measureString(
  const void* font,             // returned font
  const char* string,           // string to measure
  int len)                      // length of string, -1 for strlen
{
  if (len == -1)
    len = strlen(string);

  const mgFont* realFont = (const mgFont*) font;
  return realFont->stringWidth(string, len);
}

//--------------------------------------------------------------
// draw string at coordinates
void mgFormPane::drawString(
  const void* font,             // returned font
  long color,                   // color in RGB format
  int x,                        // x coordinate of first char
  int y,                        // y coordinate of baseline
  const char* string,           // string to measure
  int len)                      // length of string, -1 for strlen
{
  if (len == -1)
    len = strlen(string);

  const mgFont* realFont = (const mgFont*) font;
  m_drawGC->setFont(realFont);
  m_drawGC->setTextColor(mgColor((DWORD) color));
  m_drawGC->drawString(string, len, x, y);

#ifdef SHOWBASELINE
  m_drawGC->drawLine(x, y-realFont->getAscent(), x+100, y-realFont->getAscent());
#endif
}

//--------------------------------------------------------------
// reset the children of a child
void mgFormPane::childReset(
  const void* child)
{
}

//--------------------------------------------------------------
// get width range of child
void mgFormPane::childWidthRange(
  const void* child,
  int& minWidth,
  int& maxWidth)
{
  mgControl* cntl = (mgControl*) child;

#ifdef TRACE_TABLES
  mgDebug("formatter childWidthRange...");
#endif

  // get size range of control
  mgDimension minSize, maxSize;
  cntl->minimumSize(minSize);
  cntl->preferredSize(maxSize);

  minWidth = minSize.m_width;
  maxWidth = maxSize.m_width;
#ifdef TRACE_TABLES
  mgDebug("formatter childWidthRange is %d to %d", minWidth, maxWidth);
#endif
}

//--------------------------------------------------------------
// get preferred size at width
void mgFormPane::childSizeAtWidth(
  const void* child,
  int width,
  mgDimension& size)
{
  mgControl* cntl = (mgControl*) child;

#ifdef TRACE_TABLES
  mgDebug("formatter childSizeAtWidth %d...", width);
#endif
  // get preferred size of control
  if (!cntl->preferredSizeAtWidth(width, size))
    cntl->preferredSize(size);

#ifdef TRACE_TABLES
  mgDebug("formatter childSizeAtWidth(%d) is %d by %d", width, size.m_width, size.m_height);
#endif
}

//--------------------------------------------------------------
// set child position
void mgFormPane::childSetPosition(
  const void* child,
  int x,
  int y,
  int width,
  int height)
{
  mgControl* cntl = (mgControl*) child;
  cntl->setLocation(x, y);
  cntl->setSize(width, height);  
}

//--------------------------------------------------------------
// return minimum size
void mgFormPane::minimumSize(
  mgDimension& size)
{
#ifdef TRACE_TABLES
  mgDebug("FormPane minimumSize...");
#endif
  // tell the formatter to use tiny page width.  Result should be max of 
  // requested width and required width.
  mgTextFormat format(m_text, (mgTextPage*) this, true, 0, 0, 10);
  unsigned int posn = 0;
  format.scan(posn);

  size.m_width = format.m_boxWidth;
//  size.m_height = format.m_boxHeight;
// =-= table resize of text boxes still broken.  keep demos working with this change.
  size.m_height = format.m_lineHeight;

  if (m_frame != NULL)
  {
    mgDimension exterior;
    m_frame->getOutsideSize(size, exterior);
    size = exterior;
  }
#ifdef TRACE_TABLES
  mgDebug("FormPane minimumSize is (%d by %d)", size.m_width, size.m_height);
#endif
}

//--------------------------------------------------------------
// return preferred size
void mgFormPane::preferredSize(
  mgDimension& size)
{
#ifdef TRACE_TABLES
  mgDebug("FormPane preferredSize...");
#endif

  // tell the formatter to use giant page width.  Result should be min of 
  // requested width and required width.
  mgTextFormat format(m_text, (mgTextPage*) this, true, 0, 0, 10000);
  unsigned int posn = 0;
  format.scan(posn);

  size.m_width = format.m_boxWidth;
  size.m_height = format.m_boxHeight;

  if (m_frame != NULL)
  {
    mgDimension exterior;
    m_frame->getOutsideSize(size, exterior);
    size = exterior;
  }
#ifdef TRACE_TABLES
  mgDebug("FormPane preferredSize is (%d by %d)", size.m_width, size.m_height);
#endif
}

//--------------------------------------------------------------
// compute size at width.  return false if not implemented
BOOL mgFormPane::preferredSizeAtWidth(
  int width,
  mgDimension& size)
{
#ifdef TRACE_TABLES
  mgDebug("FormPane preferredSizeAtWidth(%d)...", width);
#endif
  // subtract frame from requested width
  if (m_frame != NULL)
  {
    mgRectangle rect(0, 0, width, width);
    m_frame->getInsideRect(rect);
    width = rect.m_width;
  }

  // format at requested width
  mgTextFormat format(m_text, (mgTextPage*) this, true, 0, 0, width);
  unsigned int posn = 0;
  format.scan(posn);

  size.m_width = format.m_boxWidth;
  size.m_height = format.m_boxHeight;

  if (m_frame != NULL)
  {
    mgDimension exterior;
    m_frame->getOutsideSize(size, exterior);
    size = exterior;
  }

#ifdef TRACE_TABLES
  mgDebug("FormPane preferredSizeAtWidth(%d) is (%d by %d)", width, size.m_width, size.m_height);
#endif
  return true;
}

//--------------------------------------------------------------
// paint content of control
void mgFormPane::paint(
  mgContext* gc)
{
  mgDimension size;
  getSize(size);

  mgRectangle interior(0, 0, size.m_width, size.m_height);
  if (m_frame != NULL)
  {
    m_frame->paintBackground(gc, 0, 0, size.m_width, size.m_height);
    m_frame->getInsideRect(interior);
  }

  m_drawGC = gc;

//#define SHOWBACK
#ifdef SHOWBACK
  // =-= debug
  gc->setBrush(getSurface()->createBrush(mgColor(255-m_defaultTextColor.m_r, 255-m_defaultTextColor.m_g, 255-m_defaultTextColor.m_b)));
  gc->fillRect(1, 1, size.m_width-2, size.m_height-2);
#endif
//#define SHOWEDGE
#ifdef SHOWEDGE
  // =-= debug
  gc->setPen(getSurface()->createPen(mgColor("green"), 1));
  gc->drawRect(1, 1, size.m_width-2, size.m_height-2);
#endif

  gc->setDrawMode(MG_DRAW_RGB);
#ifdef TRACE_TABLES
mgDebug("FormPane interior = (%d, %d) (%d by %d)", interior.m_x, interior.m_y, interior.m_width, interior.m_height);
#endif

  mgTextDraw draw(m_text, this, interior, interior.m_x, interior.m_y, interior.m_width);
  unsigned int posn = 0;
  draw.scan(posn);

//#define SHOWPAGE
#ifdef SHOWPAGE
  gc->setPen(getSurface()->createPen(mgColor("red"), 1));
  gc->drawLine(5, draw.m_boxHeight-3, size.m_width-3, draw.m_boxHeight-3);
  gc->drawLine(draw.m_boxWidth-3, 5, draw.m_boxWidth-3, size.m_height-3);
#endif

  m_drawGC = NULL;

  if (m_frame != NULL)
  {
    m_frame->paintForeground(gc, 0, 0, size.m_width, size.m_height);
  }

}
