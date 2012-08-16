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

#include "mgXPSurface.h"
#include "mgXPContext.h"
#include "mgXPPen.h"
#include "mgXPBrush.h"
#include "mgXPFont.h"
#include "mgXPImage.h"
#include "mgXPIcon.h"
#include "Graphics2D/mgColor.h"

/*
  We draw the RGB+alpha graphics we need for a 3d texture by having two
  bitmaps, one for color and another for alpha.  We draw into both of them
  to produce the effects we want.  Then bytes from both bitmaps are combined
  to make the final texture.  This is gross and should be rewritten.
  Direct2D might do the trick (although text looks problematic) if we 
  want to limit ourselves to Windows 7 only.
*/

class mgXPGraphicsState
{
public:
  RECT m_clipBounds;
  XFORM m_transform;
  BOOL m_paintMode;
  int m_drawMode;
  const mgXPFont* m_font;
  const mgXPBrush* m_brush;
  const mgXPPen* m_pen;
  mgColor m_color;
  int m_alpha;
};

//--------------------------------------------------------------
// constructor
mgXPContext::mgXPContext(
  mgXPSurface* surface)
{
  m_surface = surface;

  m_surfaceDC = m_surface->m_surfaceDC;
  m_alphaDC = m_surface->m_alphaDC;
  m_imageDC = m_surface->m_imageDC;

  m_defaultFont = (const mgXPFont*) m_surface->createFont("Arial", 10, false, false);
  m_defaultPen = (const mgXPPen*) m_surface->createPen(0, 0, 0, 1);
  m_defaultBrush = (const mgXPBrush*) m_surface->createBrush(0, 0, 0);
  
  resetState();

  // create alpha pen and brush
  DWORD color = RGB(m_alpha, m_alpha, m_alpha);
  m_alphaPen = CreatePen(PS_SOLID, 1, color);
  m_alphaBrush = CreateSolidBrush(color);
}

//--------------------------------------------------------------
// destructor
mgXPContext::~mgXPContext()
{
}

//--------------------------------------------------------------
// make this the current context on the surface
void mgXPContext::makeCurrent()
{
  if (m_surface->m_currentGC == this)
    return;

  SetWorldTransform(m_surfaceDC, &m_transform);
  SetWorldTransform(m_alphaDC, &m_transform);

  SelectClipRgn(m_surfaceDC, NULL);
  IntersectClipRect(m_surfaceDC, m_clipBounds.left, m_clipBounds.top, m_clipBounds.right, m_clipBounds.bottom);

  SelectClipRgn(m_alphaDC, NULL);
  IntersectClipRect(m_alphaDC, m_clipBounds.left, m_clipBounds.top, m_clipBounds.right, m_clipBounds.bottom);

  if (m_paintMode)
  {
    SetROP2(m_surfaceDC, R2_COPYPEN);
    SetROP2(m_alphaDC, R2_COPYPEN);
  }
  else
  {
    SetROP2(m_surfaceDC, R2_XORPEN);
    SetROP2(m_alphaDC, R2_COPYPEN);
    // =-= set pen color to XOR of current color and given color
  }
  
  SelectObject(m_surfaceDC, m_font->m_platformFont);
  SelectObject(m_surfaceDC, m_brush->m_platformBrush);
  SelectObject(m_surfaceDC, m_pen->m_platformPen);
  SetTextColor(m_surfaceDC, RGB(m_color.m_r, m_color.m_g, m_color.m_b));

  SelectObject(m_alphaDC, m_font->m_platformFont);
  SelectObject(m_alphaDC, m_alphaBrush);
  SelectObject(m_alphaDC, m_alphaPen);
  SetTextColor(m_alphaDC, RGB(m_alpha, m_alpha, m_alpha));

  m_surface->m_currentGC = this;
}
    
//--------------------------------------------------------------
// reset drawing state
void mgXPContext::resetState()
{
  m_paintMode = true;
  m_drawMode = MG_DRAW_ALL;

  m_font = m_defaultFont;
 
  m_pen = m_defaultPen;
  m_brush = m_defaultBrush;
  m_color = mgColor("black");

  // set identity transform
  m_transform.eM11 = 1.0f;
  m_transform.eM12 = 0.0f; 
  m_transform.eM21 = 0.0f; 
  m_transform.eM22 = 1.0f; 
  m_transform.eDx  = 0.0f; 
  m_transform.eDy  = 0.0f;

  // initialize clip
  m_clipBounds.left = 0;
  m_clipBounds.top = 0;
  m_clipBounds.right = 32768; // =-= device size?
  m_clipBounds.bottom = 32768; // =-= device size?

  m_alpha = 255;  // opaque
  DWORD color = RGB(m_alpha, m_alpha, m_alpha);
  m_alphaPen = CreatePen(PS_SOLID, 1, color);
  m_alphaBrush = CreateSolidBrush(color);

  // set the new state on the surface next time we draw
  m_surface->m_currentGC = NULL;
}

//--------------------------------------------------------------
// return current state
void* mgXPContext::getState()
{
  mgXPGraphicsState* state = new mgXPGraphicsState();

  state->m_transform = m_transform;
  state->m_clipBounds = m_clipBounds;
  state->m_paintMode = m_paintMode;
  state->m_drawMode = m_drawMode;

  state->m_brush = m_brush;
  state->m_pen = m_pen;
  state->m_font = m_font;
  state->m_color = m_color;
  state->m_alpha = m_alpha;

  return state;
}
  
//--------------------------------------------------------------
// restore previous state
void mgXPContext::setState(
  void* s)
{
  mgXPGraphicsState* state = (mgXPGraphicsState*) s;
  
  m_transform = state->m_transform;
  m_clipBounds = state->m_clipBounds;
  m_paintMode = state->m_paintMode;
  m_drawMode = state->m_drawMode;

  m_brush = state->m_brush;
  m_pen = state->m_pen;
  m_font = state->m_font;
  m_color = state->m_color;
  m_alpha = state->m_alpha;

  DWORD color = RGB(m_alpha, m_alpha, m_alpha);
  m_alphaPen = CreatePen(PS_SOLID, 1, color);
  m_alphaBrush = CreateSolidBrush(color);

  // set the new state on the surface next time we draw
  m_surface->m_currentGC = NULL;
}

//--------------------------------------------------------------
// delete saved state
void mgXPContext::deleteState(
  void* state)
{
  delete (mgXPGraphicsState*) state;
}

//--------------------------------------------------------------
// set drawing mode
void mgXPContext::setDrawMode(
  int mode)
{
  m_drawMode = mode;
}
  
//--------------------------------------------------------------
// set clipping rectangle
void mgXPContext::setClip(
  int x,
  int y, 
  int width,
  int height)
{
  makeCurrent();

  IntersectClipRect(m_surfaceDC, x, y, x+width, y+height);
  IntersectClipRect(m_alphaDC, x, y, x+width, y+height);
  GetClipBox(m_surfaceDC, &m_clipBounds);

  // =-= clipBounds should be a region, since it could be a rotated clip by the
  // interface of mgContext.
}

//--------------------------------------------------------------
// return box bounding current clip
void mgXPContext::getClip(
  mgRectangle& bounds)
{
  bounds.m_x = m_clipBounds.left;
  bounds.m_y = m_clipBounds.top;
  bounds.m_width = m_clipBounds.right - m_clipBounds.left;
  bounds.m_height = m_clipBounds.bottom - m_clipBounds.top;
}

//--------------------------------------------------------------
// translate coordinate system
void mgXPContext::translate(
  int x,
  int y)
{
  makeCurrent();
  // build translate xform
  XFORM translate;
  translate.eM11 = 1.0f;
  translate.eM12 = 0.0f; 
  translate.eM21 = 0.0f; 
  translate.eM22 = 1.0f; 
  translate.eDx  = (FLOAT) x;
  translate.eDy  = (FLOAT) y;

  ModifyWorldTransform(m_surfaceDC, &translate, MWT_LEFTMULTIPLY);
  ModifyWorldTransform(m_alphaDC, &translate, MWT_LEFTMULTIPLY);

  // get clip bounds under current transform
  GetWorldTransform(m_surfaceDC, &m_transform);
  GetClipBox(m_surfaceDC, &m_clipBounds);
}

#ifdef WORKED
//--------------------------------------------------------------------
// void GRContext.rotate(double)
void mgXPContext::rotate(
  double radians)
{
  makeCurrent();
  // build rotation xform
  XFORM rotate;
  rotate.eM11 = (FLOAT) cos(radians);
  rotate.eM12 = (FLOAT) sin(radians); 
  rotate.eM21 = -rotate.eM12;
  rotate.eM22 = rotate.eM11; 
  rotate.eDx  = 0.0f;
  rotate.eDy  = 0.0f;

  ModifyWorldTransform(m_surfaceDC, &rotate, MWT_LEFTMULTIPLY);
  ModifyWorldTransform(m_alphaDC, &rotate, MWT_LEFTMULTIPLY);

  // get clip bounds under current transform
  GetWorldTransform(m_surfaceDC, &m_transform);
  GetClipBox(m_surfaceDC, &m_clipBounds);
}
#endif

//--------------------------------------------------------------
// set pen for lines
void mgXPContext::setPen(
  const mgPen* pen)
{
  makeCurrent();
  m_pen = (mgXPPen*) pen;
  SelectObject(m_surfaceDC, m_pen->m_platformPen);
}
  
//--------------------------------------------------------------
// set brush for fill
void mgXPContext::setBrush(
  const mgBrush* brush)
{
  makeCurrent();
  m_brush = (mgXPBrush*) brush;
  SelectObject(m_surfaceDC, m_brush->m_platformBrush);
}
  
//--------------------------------------------------------------
// set font for text
void mgXPContext::setFont(
  const mgFont* font)
{
  makeCurrent();
  m_font = (mgXPFont *) font;
  SelectObject(m_surfaceDC, m_font->m_platformFont);
  SelectObject(m_alphaDC, m_font->m_platformFont);
}
  
//--------------------------------------------------------------
// set color for text
void mgXPContext::setTextColor(
  const mgColor& color)
{
  makeCurrent();
  m_color = color;
  SetTextColor(m_surfaceDC, RGB(m_color.m_r, m_color.m_g, m_color.m_b));
}

//--------------------------------------------------------------
// set alpha plane value
void mgXPContext::setAlpha(
  int alpha)
{
  if (alpha == m_alpha)
    return;

  makeCurrent();

  // delete the old brush and pen
  SelectObject(m_alphaDC, (HPEN) NULL);
  SelectObject(m_alphaDC, (HBRUSH) NULL);
  DeleteObject(m_alphaPen);
  DeleteObject(m_alphaBrush);

  m_alpha = alpha;
  DWORD color = RGB(m_alpha, m_alpha, m_alpha);

  m_alphaPen = CreatePen(PS_SOLID, 1, color);

  m_alphaBrush = CreateSolidBrush(color);
  SelectObject(m_alphaDC, m_alphaPen);
  SelectObject(m_alphaDC, m_alphaBrush);

  SetTextColor(m_alphaDC, color);
}
        
//--------------------------------------------------------------
// draw line segment
void mgXPContext::drawLine(
  int fx, 
  int fy, 
  int tx, 
  int ty)
{
  makeCurrent();
  if ((m_drawMode & MG_DRAW_RGB) != 0)
  {
    MoveToEx(m_surfaceDC, fx, fy, NULL);
    LineTo(m_surfaceDC, tx, ty);
  }

  if ((m_drawMode & MG_DRAW_ALPHA) != 0)
  {
    MoveToEx(m_alphaDC, fx, fy, NULL);
    LineTo(m_alphaDC, tx, ty);  
  }
}
  
//--------------------------------------------------------------
// draw rectangle
void mgXPContext::drawRect(
  int x, 
  int y, 
  int width, 
  int height)
{
  makeCurrent();
  // draw fill rect with null brush
  if ((m_drawMode & MG_DRAW_RGB) != 0)
  {
    SelectObject(m_surfaceDC, GetStockObject(NULL_BRUSH));
    Rectangle(m_surfaceDC, x, y, x+width-1, y+height-1);

    SelectObject(m_surfaceDC, m_brush->m_platformBrush);
  }

  if ((m_drawMode & MG_DRAW_ALPHA) != 0)
  {
    SelectObject(m_alphaDC, GetStockObject(NULL_BRUSH));
    Rectangle(m_alphaDC, x, y, x+width-1, y+height-1);  

    SelectObject(m_alphaDC, m_alphaPen);
  }
}
  
//--------------------------------------------------------------
// fill rectangle
void mgXPContext::fillRect(
  int x,
  int y,
  int width,
  int height)
{
  makeCurrent();
  // can't use FillRect here, since it doesn't honor XorMode.
  if ((m_drawMode & MG_DRAW_RGB) != 0)
  {
    SelectObject(m_surfaceDC, m_brush->m_platformBrush);
    SelectObject(m_surfaceDC, m_surface->m_nullPen);
    Rectangle(m_surfaceDC, x, y, x+width, y+height);

    SelectObject(m_surfaceDC, m_pen->m_platformPen);
  }

  if ((m_drawMode & MG_DRAW_ALPHA) != 0)
  {
    SelectObject(m_alphaDC, m_alphaBrush);
    SelectObject(m_alphaDC, m_surface->m_nullPen);
    Rectangle(m_alphaDC, x, y, x+width, y+height);

    SelectObject(m_alphaDC, m_alphaPen);
  }
}

//--------------------------------------------------------------
// draw an image
void mgXPContext::drawImage(
  const mgImage* image,
  int dx,               // destination top-left
  int dy,
  int dwd,              // destination size
  int dht,
  int sx,               // source top-left
  int sy,
  int swd,              // source size
  int sht)
{
  makeCurrent();
  mgXPImage* xpImage = (mgXPImage*) image;

  switch (m_drawMode)
  {
    case MG_DRAW_RGB:
      drawImageToDC(m_surfaceDC, xpImage->m_platformRGB, dx, dy, dwd, dht, sx, sy, swd, sht);
      break;

    case MG_DRAW_ALPHA:
      drawImageToDC(m_alphaDC, xpImage->m_platformAlpha, dx, dy, dwd, dht, sx, sy, swd, sht);
      break;

    case MG_DRAW_ALL:
      drawImageToDC(m_surfaceDC, xpImage->m_platformRGB, dx, dy, dwd, dht, sx, sy, swd, sht);
      drawImageToDC(m_alphaDC, xpImage->m_platformAlpha, dx, dy, dwd, dht, sx, sy, swd, sht);
      break;
  }
}
        
//--------------------------------------------------------------
// draw an image
void mgXPContext::drawImage(
  const mgImage* image,
  int x,              // destination top-left
  int y)
{
  int imgWidth = image->getWidth();
  int imgHeight = image->getHeight();
  drawImage(image, x, y, imgWidth, imgHeight, 0, 0, imgWidth, imgHeight);
}
        
//--------------------------------------------------------------
// draw an image
void mgXPContext::drawImage(
  const mgImage* image,
  int x,              // destination top-left
  int y,
  int width,
  int height)
{
  int imgWidth = image->getWidth();
  int imgHeight = image->getHeight();
  drawImage(image, x, y, width, height, 0, 0, imgWidth, imgHeight);
}
        
//--------------------------------------------------------------
// draw an image scaled to fit rectangle
void mgXPContext::drawImageWithin(
  const mgImage* image,
  int x,
  int y,
  int width,
  int height)
{
  int imgWidth = image->getWidth();
  int imgHeight = image->getHeight();

  if (imgWidth == 0 || imgHeight == 0)
    return;
    
  int targetWidth = width;
  int targetHeight = height;
  if (imgWidth * height > imgHeight * width)
  {
    // constrained by width
    targetHeight = (targetWidth * imgHeight)/imgWidth;
  }
  else
  {
    // constrained by height
    targetWidth = (targetHeight * imgWidth)/imgHeight;
  }

  int targetX = x+(width-targetWidth)/2;
  int targetY = y+(height-targetHeight)/2;

  drawImage(image, targetX, targetY, targetWidth, targetHeight, 0, 0, imgWidth, imgHeight);
}

//--------------------------------------------------------------
// draw an icon
void mgXPContext::drawIcon(
  const mgIcon* icon,
  int dx,               // destination top-left
  int dy,
  int dwd,              // destination size
  int dht,
  int sx,               // source top-left
  int sy,
  int swd,              // source size
  int sht)
{
  makeCurrent();
  mgXPIcon* xpIcon = (mgXPIcon*) icon;

  switch (m_drawMode)
  {
    case MG_DRAW_RGB:
      drawImageToDC(m_surfaceDC, xpIcon->m_platformRGB, dx, dy, dwd, dht, sx, sy, swd, sht);
      break;

    case MG_DRAW_ALPHA:
      drawImageToDC(m_alphaDC, xpIcon->m_platformAlpha, dx, dy, dwd, dht, sx, sy, swd, sht);
      break;

    case MG_DRAW_ALL:
      drawImageToDC(m_surfaceDC, xpIcon->m_platformRGB, dx, dy, dwd, dht, sx, sy, swd, sht);
      drawImageToDC(m_alphaDC, xpIcon->m_platformAlpha, dx, dy, dwd, dht, sx, sy, swd, sht);
      break;
  }
}
        
//--------------------------------------------------------------
// draw an icon
void mgXPContext::drawIcon(
  const mgIcon* icon,
  int x,              // destination top-left
  int y)
{
  int iconWidth = icon->getWidth();
  int iconHeight = icon->getHeight();
  drawIcon(icon, x, y, iconWidth, iconHeight, 0, 0, iconWidth, iconHeight);
}
        
//--------------------------------------------------------------
// draw an icon
void mgXPContext::drawIcon(
  const mgIcon* icon,
  int x,              // destination top-left
  int y,
  int width,
  int height)
{
  int iconWidth = icon->getWidth();
  int iconHeight = icon->getHeight();
  drawIcon(icon, x, y, width, height, 0, 0, iconWidth, iconHeight);
}
        
//--------------------------------------------------------------
// draw an icon scaled to fit rectangle
void mgXPContext::drawIconWithin(
  const mgIcon* icon,
  int x,
  int y,
  int width,
  int height)
{
  int iconWidth = icon->getWidth();
  int iconHeight = icon->getHeight();

  if (iconWidth == 0 || iconHeight == 0)
    return;
    
  int targetWidth = width;
  int targetHeight = height;
  if (iconWidth * height > iconHeight * width)
  {
    // constrained by width
    targetHeight = (targetWidth * iconHeight)/iconWidth;
  }
  else
  {
    // constrained by height
    targetWidth = (targetHeight * iconWidth)/iconHeight;
  }

  int targetX = x+(width-targetWidth)/2;
  int targetY = y+(height-targetHeight)/2;

  drawIcon(icon, targetX, targetY, targetWidth, targetHeight, 0, 0, iconWidth, iconHeight);
}

//--------------------------------------------------------------
// draw an image to a DC
void mgXPContext::drawImageToDC(
  HDC targetDC,
  HBITMAP imageBitmap,
  int dx,               // destination top-left
  int dy,
  int dwd,              // destination size
  int dht,
  int sx,               // source top-left
  int sy,
  int swd,              // source size
  int sht)
{
  SelectObject(m_imageDC, imageBitmap);

  StretchBlt(targetDC, dx, dy, dwd, dht, 
             m_imageDC, sx, sy, swd, sht, SRCCOPY);

  SelectObject(m_imageDC, (HBITMAP) NULL);
}
        
//--------------------------------------------------------------
// draw a string
void mgXPContext::drawString(
  const char* text,
  int textLen,
  int x,
  int y)
{
  m_surface->m_currentGC = NULL;
  makeCurrent();

  mgString str(text, textLen);
  int outLen;
  WCHAR* outText;
  str.toWCHAR(outText, outLen);

  // draw text with solid brush
  if ((m_drawMode & MG_DRAW_RGB) != 0)
  {
    SelectObject(m_surfaceDC, (HBRUSH) NULL);
    TextOut(m_surfaceDC, x, y, outText, outLen);
    SelectObject(m_surfaceDC, m_brush->m_platformBrush);
  }

  if ((m_drawMode & MG_DRAW_ALPHA) != 0)
  {
    SelectObject(m_alphaDC, (HBRUSH) NULL);
    TextOut(m_alphaDC, x, y, outText, outLen);
    SelectObject(m_alphaDC, m_brush->m_platformBrush);
  }

  delete outText;
}

//--------------------------------------------------------------
// get box around a string
void mgXPContext::stringExtent(
  const char* text,
  int textLen,
  mgPoint& endPt,
  mgRectangle& bounds)
{
  makeCurrent();

  mgString str(text, textLen);
  int outLen;
  WCHAR* outText;
  str.toWCHAR(outText, outLen);

  // =-= should figure leading a-space (might be neg) and trailing c-space
  SIZE size;
  GetTextExtentPoint32(m_surfaceDC, outText, outLen, &size);
  
  delete outText;

  // advance of current point
  endPt.m_x = size.cx;
  endPt.m_y = size.cy;
  
  // bounding rectangle of modified bits
  bounds.m_x = 0;
  bounds.m_y = -m_font->getAscent();
  bounds.m_width = size.cx;
  bounds.m_height = size.cy;
}

//--------------------------------------------------------------
// get width of string
int mgXPContext::stringWidth(
  const char* text,
  int textLen)
{
  makeCurrent();

  mgString str(text, textLen);
  int outLen;
  WCHAR* outText;
  str.toWCHAR(outText, outLen);

  SIZE size;
  GetTextExtentPoint32(m_surfaceDC, outText, outLen, &size);
  
  delete outText;
  return size.cx;
}

//--------------------------------------------------------------
// get number of chars which fit in width
int mgXPContext::stringFit(
  const char* text,
  int textLen,
  int width)
{
  makeCurrent();

  mgString str(text, textLen);
  int outLen;
  WCHAR* outText;
  str.toWCHAR(outText, outLen);

  int fit;
  SIZE size;
  GetTextExtentExPoint(m_surfaceDC, outText, outLen, width, &fit, NULL, &size);

  delete outText;
  
  // the fit count is in unicode.  we have to map this back to source string
  int utfPosn = 0;
  for (int i = 0; i < fit; i++)
  {
    utfPosn = str.nextLetter(utfPosn);
  }

  return utfPosn;
}
