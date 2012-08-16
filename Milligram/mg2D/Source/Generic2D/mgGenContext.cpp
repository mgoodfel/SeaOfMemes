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

#include "Graphics2D/mgColor.h"
#include "mgGenPen.h"
#include "mgGenBrush.h"
#include "mgGenFont.h"
#include "mgGenImage.h"
#include "mgGenIcon.h"
#include "mgGenCharCache.h"
#include "Graphics2D/Surfaces/mgGenSurface.h"
#include "mgGenContext.h"

class mgGenGraphicsState
{
public:
  mgRectangle m_reqClip;
//  XFORM m_transform;
  mgPoint m_origin;
  BOOL m_paintMode;
  int m_drawMode;

  const mgGenFont* m_font;
  const mgGenBrush* m_brush;
  const mgGenPen* m_pen;
  mgColor m_textColor;
  int m_alpha;
};

//--------------------------------------------------------------
// constructor
mgGenContext::mgGenContext(
  mgGenSurface* surface)
{
  m_surface = surface;

  m_defaultFont = (const mgGenFont*) m_surface->createFont("Arial", 10, false, false);
  m_defaultPen = (const mgGenPen*) m_surface->createPen(0, 0, 0, 1);
  m_defaultBrush = (const mgGenBrush*) m_surface->createBrush(0, 0, 0);
  
  resetState();
}

//--------------------------------------------------------------
// destructor
mgGenContext::~mgGenContext()
{
}

//--------------------------------------------------------------
// reset drawing state
void mgGenContext::resetState()
{
  m_paintMode = true;
  m_drawMode = MG_DRAW_ALL;

  m_font = m_defaultFont;
 
  m_pen = m_defaultPen;
  m_brush = m_defaultBrush;
  m_textColor = mgColor("black");
  m_alpha = 255;  // opaque

/*
  // set identity transform
  m_transform.eM11 = 1.0f;
  m_transform.eM12 = 0.0f; 
  m_transform.eM21 = 0.0f; 
  m_transform.eM22 = 1.0f; 
  m_transform.eDx  = 0.0f; 
  m_transform.eDy  = 0.0f;
*/
  m_origin.m_x = 0;
  m_origin.m_y = 0;

  // initialize clip
  m_reqClip.m_x = 0;
  m_reqClip.m_y = 0;
  m_reqClip.m_width = 32768;
  m_reqClip.m_height = 32768; 

  m_clipBounds.m_x = 0;
  m_clipBounds.m_y = 0;
  m_clipBounds.m_width = m_surface->m_imageWidth;
  m_clipBounds.m_height = m_surface->m_imageHeight;
}

//--------------------------------------------------------------
// return current state
void* mgGenContext::getState()
{
  mgGenGraphicsState* state = new mgGenGraphicsState();

//  state->m_transform = m_transform;
  state->m_origin = m_origin;
  state->m_reqClip = m_reqClip;
  state->m_paintMode = m_paintMode;
  state->m_drawMode = m_drawMode;

  state->m_brush = m_brush;
  state->m_pen = m_pen;
  state->m_font = m_font;
  state->m_textColor = m_textColor;
  state->m_alpha = m_alpha;

  return state;
}
  
//--------------------------------------------------------------
// restore previous state
void mgGenContext::setState(
  void* s)
{
  mgGenGraphicsState* state = (mgGenGraphicsState*) s;
  
//  m_transform = state->m_transform;
  m_origin = state->m_origin;

  // intersect requested clip bounds with surface size
  m_reqClip = state->m_reqClip;
  m_clipBounds.m_x = max(0, m_reqClip.m_x);
  m_clipBounds.m_y = max(0, m_reqClip.m_y);
  int clipRight = m_reqClip.m_x + m_reqClip.m_width;
  int clipBottom = m_reqClip.m_y + m_reqClip.m_height;
  m_clipBounds.m_width = min(m_surface->m_imageWidth, clipRight) - m_clipBounds.m_x;
  m_clipBounds.m_height = min(m_surface->m_imageHeight, clipBottom) - m_clipBounds.m_y;

  m_paintMode = state->m_paintMode;
  m_drawMode = state->m_drawMode;

  m_brush = state->m_brush;
  m_pen = state->m_pen;
  m_font = state->m_font;
  m_textColor = state->m_textColor;
  m_alpha = state->m_alpha;
}

//--------------------------------------------------------------
// delete saved state
void mgGenContext::deleteState(
  void* state)
{
  delete (mgGenGraphicsState*) state;
}

//--------------------------------------------------------------
// set drawing mode
void mgGenContext::setDrawMode(
  int mode)
{
  m_drawMode = mode;
}
  
//--------------------------------------------------------------
// set clipping rectangle
void mgGenContext::setClip(
  int x,
  int y, 
  int width,
  int height)
{
  // intersect with existing clip
  int left = max(m_origin.m_x + x, m_reqClip.m_x);
  int right = min(m_origin.m_x + x+width, m_reqClip.m_x+m_reqClip.m_width);
  int top = max(m_origin.m_y + y, m_reqClip.m_y);
  int bottom = min(m_origin.m_y + y+height, m_reqClip.m_y+m_reqClip.m_height);

  m_reqClip.m_x = left;
  m_reqClip.m_y = top;
  m_reqClip.m_width = max(0, right-left);
  m_reqClip.m_height = max(0, bottom-top);

  // intersect with surface bounds to get real clip rectangle
  m_clipBounds.m_x = max(0, left);
  m_clipBounds.m_y = max(0, top);
  m_clipBounds.m_width = min(m_surface->m_imageWidth, right) - m_clipBounds.m_x;
  m_clipBounds.m_height = min(m_surface->m_imageHeight, bottom) - m_clipBounds.m_y;
}

//--------------------------------------------------------------
// return box bounding current clip
void mgGenContext::getClip(
  mgRectangle& bounds)
{
  bounds.m_x = m_clipBounds.m_x - m_origin.m_x;
  bounds.m_y = m_clipBounds.m_y - m_origin.m_y;
  bounds.m_width = m_clipBounds.m_width;
  bounds.m_height = m_clipBounds.m_height;
}

//--------------------------------------------------------------
// translate coordinate system
void mgGenContext::translate(
  int x,
  int y)
{
  m_origin.m_x += x;
  m_origin.m_y += y;
}

#ifdef WORKED
//--------------------------------------------------------------------
// void GRContext.rotate(double)
void mgGenContext::rotate(
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
void mgGenContext::setPen(
  const mgPen* pen)
{
  m_pen = (mgGenPen*) pen;
}
  
//--------------------------------------------------------------
// set brush for fill
void mgGenContext::setBrush(
  const mgBrush* brush)
{
  m_brush = (mgGenBrush*) brush;
}
  
//--------------------------------------------------------------
// set font for text
void mgGenContext::setFont(
  const mgFont* font)
{
  m_font = (mgGenFont *) font;
}
  
//--------------------------------------------------------------
// set color for text
void mgGenContext::setTextColor(
  const mgColor& color)
{
  m_textColor = color;
}

//--------------------------------------------------------------
// set alpha plane value
void mgGenContext::setAlpha(
  int alpha)
{
  m_alpha = alpha;
}
        
//--------------------------------------------------------------
// fill rectangle in rgb planes.  
// bounds already clipped, in surface coords
void mgGenContext::fillRectRGB(
  int rectLeft,
  int rectTop,
  int rectRight,
  int rectBottom,
  DWORD pixel)
{
  // fill the rectangle
  DWORD* surfaceLine = m_surface->m_imageData + m_surface->m_imageWidth*rectTop;
  for (int y = rectTop; y < rectBottom; y++)
  {
    for (int j = rectLeft; j < rectRight; j++)
      surfaceLine[j] = (0xFF000000 & surfaceLine[j]) | pixel;

    surfaceLine += m_surface->m_imageWidth;
  }
}

//--------------------------------------------------------------
// fill rectangle in rgb planes.  
// bounds already clipped, in surface coords
void mgGenContext::fillRectRGBA(
  int rectLeft,
  int rectTop,
  int rectRight,
  int rectBottom,
  DWORD pixel)
{
  // fill the rectangle
  DWORD* surfaceLine = m_surface->m_imageData + m_surface->m_imageWidth*rectTop;
  for (int y = rectTop; y < rectBottom; y++)
  {
    for (int j = rectLeft; j < rectRight; j++)
      surfaceLine[j] = pixel;

    surfaceLine += m_surface->m_imageWidth;
  }
}

//--------------------------------------------------------------
// fill rectangle in alpha planes.  
// bounds already clipped, in surface coords
void mgGenContext::fillRectAlpha(
  int rectLeft,
  int rectTop,
  int rectRight,
  int rectBottom,
  BYTE alpha)
{
  DWORD pixel = alpha << 24;
  // fill the rectangle
  DWORD* surfaceLine = m_surface->m_imageData + m_surface->m_imageWidth*rectTop;
  for (int y = rectTop; y < rectBottom; y++)
  {
    for (int j = rectLeft; j < rectRight; j++)
      surfaceLine[j] = (0x00FFFFFF & surfaceLine[j]) | pixel;

    surfaceLine += m_surface->m_imageWidth;
  }
}

//--------------------------------------------------------------
// draw line segment
void mgGenContext::drawLine(
  int fx, 
  int fy, 
  int tx, 
  int ty)
{
  // =-= hack horizontal and vertical lines
  fx += m_origin.m_x;
  fy += m_origin.m_y;
  tx += m_origin.m_x;
  ty += m_origin.m_y;

  int rectLeft = min(fx, tx) - m_pen->m_thick/2;
  int rectRight = max(fx, tx) + m_pen->m_thick - m_pen->m_thick/2;
  int rectTop = min(fy, ty) - m_pen->m_thick/2;
  int rectBottom = max(fy, ty) + m_pen->m_thick - m_pen->m_thick/2;

  int clipLeft = m_clipBounds.m_x;
  int clipTop = m_clipBounds.m_y;
  int clipRight = m_clipBounds.right();
  int clipBottom = m_clipBounds.bottom();

  // find bounds of the four edges, intersected with clip

  // top edge
  int lx = max(rectLeft, clipLeft);
  int hx = min(rectRight, clipRight);
  int ly = max(rectTop, clipTop);
  int hy = min(rectBottom, clipBottom);

  // draw the line
  DWORD pixel;
  switch (m_drawMode)
  {
    case MG_DRAW_RGB: 
      pixel = (m_pen->m_b) |
              (m_pen->m_g << 8) |
              (m_pen->m_r << 16);

      if (lx < hx && ly < hy)
        fillRectRGB(lx, ly, hx, hy, pixel);
      break;

    case MG_DRAW_ALPHA:
      if (lx < hx && ly < hy)
        fillRectAlpha(lx, ly, hx, hy, m_alpha);
      break;

    case MG_DRAW_ALL:
      pixel = (m_pen->m_b) |
              (m_pen->m_g << 8) |
              (m_pen->m_r << 16) |
              (m_alpha << 24);
      if (lx < hx && ly < hy)
        fillRectRGBA(lx, ly, hx, hy, pixel);
  }
}
  
//--------------------------------------------------------------
// draw rectangle
void mgGenContext::drawRect(
  int rectX, 
  int rectY, 
  int rectWidth, 
  int rectHeight)
{
  rectX += m_origin.m_x;
  int rectRight = rectX + rectWidth;
  rectY += m_origin.m_y;
  int rectBottom = rectY + rectHeight;

  int clipLeft = m_clipBounds.m_x;
  int clipTop = m_clipBounds.m_y;
  int clipRight = m_clipBounds.right();
  int clipBottom = m_clipBounds.bottom();

  // find bounds of the four edges, intersected with clip

  // top edge
  int toplx = max(rectX, clipLeft);
  int tophx = min(rectRight, clipRight);
  int toply = max(rectY, clipTop);
  int tophy = min(rectY + m_pen->m_thick, clipBottom);

  // left edge
  int leftlx = max(rectX, clipLeft);
  int lefthx = min(rectX + m_pen->m_thick, clipRight);
  int leftly = max(rectY, clipTop);
  int lefthy = min(rectBottom, clipBottom);

  // bottom edge
  int bottomlx = max(rectX, clipLeft);
  int bottomhx = min(rectRight, clipRight);
  int bottomly = max(rectBottom - m_pen->m_thick, clipTop);
  int bottomhy = min(rectBottom, clipBottom);

  // right edge
  int rightlx = max(rectRight-m_pen->m_thick, clipLeft);
  int righthx = min(rectRight, clipRight);
  int rightly = max(rectY, clipTop);
  int righthy = min(rectBottom, clipBottom);


  // draw the rectangle edges
  DWORD pixel;
  switch (m_drawMode)
  {
    case MG_DRAW_RGB: 
      pixel = (m_pen->m_b) |
              (m_pen->m_g << 8) |
              (m_pen->m_r << 16);

      if (toplx < tophx && toply < tophy)
        fillRectRGB(toplx, toply, tophx, tophy, pixel);

      if (leftlx < lefthx && leftly < lefthy)
        fillRectRGB(leftlx, leftly, lefthx, lefthy, pixel);

      if (bottomlx < bottomhx && bottomly < bottomhy)
        fillRectRGB(bottomlx, bottomly, bottomhx, bottomhy, pixel);

      if (rightlx < righthx && rightly < righthy)
        fillRectRGB(rightlx, rightly, righthx, righthy, pixel);
      break;

    case MG_DRAW_ALPHA:
      if (toplx < tophx && toply < tophy)
        fillRectAlpha(toplx, toply, tophx, tophy, m_alpha);

      if (leftlx < lefthx && leftly < lefthy)
        fillRectAlpha(leftlx, leftly, lefthx, lefthy, m_alpha);

      if (bottomlx < bottomhx && bottomly < bottomhy)
        fillRectAlpha(bottomlx, bottomly, bottomhx, bottomhy, m_alpha);

      if (rightlx < righthx && rightly < righthy)
        fillRectAlpha(rightlx, rightly, righthx, righthy, m_alpha);
      break;

    case MG_DRAW_ALL:
      pixel = (m_pen->m_b) |
              (m_pen->m_g << 8) |
              (m_pen->m_r << 16) |
              (m_alpha << 24);
      if (toplx < tophx && toply < tophy)
        fillRectRGBA(toplx, toply, tophx, tophy, pixel);

      if (leftlx < lefthx && leftly < lefthy)
        fillRectRGBA(leftlx, leftly, lefthx, lefthy, pixel);

      if (bottomlx < bottomhx && bottomly < bottomhy)
        fillRectRGBA(bottomlx, bottomly, bottomhx, bottomhy, pixel);

      if (rightlx < righthx && rightly < righthy)
        fillRectRGBA(rightlx, rightly, righthx, righthy, pixel);
  }
}
  
//--------------------------------------------------------------
// fill rectangle
void mgGenContext::fillRect(
  int rectX,
  int rectY,
  int rectWidth,
  int rectHeight)
{
  // clip the bounds
  rectX += m_origin.m_x;
  rectY += m_origin.m_y;

  int left = max(rectX, m_clipBounds.m_x);
  int right = min(rectX+rectWidth, m_clipBounds.m_x+m_clipBounds.m_width);

  int top = max(rectY, m_clipBounds.m_y);
  int bottom = min(rectY+rectHeight, m_clipBounds.m_y+m_clipBounds.m_height);

  if (left >= right || top >= bottom)
    return;  // nothing to do

  DWORD pixel;
  switch (m_drawMode)
  {
    case MG_DRAW_RGB: 
      pixel = (m_brush->m_b) |
              (m_brush->m_g << 8) |
              (m_brush->m_r << 16);

      fillRectRGB(left, top, right, bottom, pixel);
      break;

    case MG_DRAW_ALPHA:
      fillRectAlpha(left, top, right, bottom, m_alpha);
      break;

    case MG_DRAW_ALL:
      pixel = (m_brush->m_b) |
              (m_brush->m_g << 8) |
              (m_brush->m_r << 16) |
              (m_alpha << 24);
      fillRectRGBA(left, top, right, bottom, pixel);
  }
}

//--------------------------------------------------------------
// draw an image
void mgGenContext::drawImage(
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
  mgGenImage* xpImage = (mgGenImage*) image;

  // clip the bounds
  dx += m_origin.m_x;
  dy += m_origin.m_y;

  int left = max(dx, m_clipBounds.m_x);
  int right = min(dx+dwd, m_clipBounds.m_x+m_clipBounds.m_width);

  int top = max(dy, m_clipBounds.m_y);
  int bottom = min(dy+dht, m_clipBounds.m_y+m_clipBounds.m_height);

  if (left >= right || top >= bottom)
    return;  // nothing to do

  // =-= debug
  DWORD pixel;
  switch (m_drawMode)
  {
    case MG_DRAW_RGB: 
      pixel = 0x000000FF;  // red

      fillRectRGB(left, top, right, bottom, pixel);
      break;

    case MG_DRAW_ALPHA:
      fillRectAlpha(left, top, right, bottom, 0x80);
      break;

    case MG_DRAW_ALL:
      pixel = 0xFF0000FF; // red
      fillRectRGBA(left, top, right, bottom, pixel);
  }
}
        
//--------------------------------------------------------------
// draw an image
void mgGenContext::drawImage(
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
void mgGenContext::drawImage(
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
void mgGenContext::drawImageWithin(
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
void mgGenContext::drawIcon(
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
  mgGenIcon* xpIcon = (mgGenIcon*) icon;

  // clip the bounds
  dx += m_origin.m_x;
  dy += m_origin.m_y;

  int left = max(dx, m_clipBounds.m_x);
  int right = min(dx+dwd, m_clipBounds.m_x+m_clipBounds.m_width);

  int top = max(dy, m_clipBounds.m_y);
  int bottom = min(dy+dht, m_clipBounds.m_y+m_clipBounds.m_height);

  if (left >= right || top >= bottom)
    return;  // nothing to do

  // =-= debug
  DWORD pixel;
  switch (m_drawMode)
  {
    case MG_DRAW_RGB: 
      pixel = 0x000000FF;  // red

      fillRectRGB(left, top, right, bottom, pixel);
      break;

    case MG_DRAW_ALPHA:
      fillRectAlpha(left, top, right, bottom, 0x80);
      break;

    case MG_DRAW_ALL:
      pixel = 0xFF0000FF; // red
      fillRectRGBA(left, top, right, bottom, pixel);
  }
}
        
//--------------------------------------------------------------
// draw an icon
void mgGenContext::drawIcon(
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
void mgGenContext::drawIcon(
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
void mgGenContext::drawIconWithin(
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
// draw a string
void mgGenContext::drawString(
  const char* text,
  int textLen,
  int x,
  int y)
{
  switch (m_drawMode)
  {
    case MG_DRAW_RGB: 
      drawStringRGB(text, textLen, x, y);
      break;
    case MG_DRAW_ALPHA:
      drawStringAlpha(text, textLen, x, y);
      break;
    default:
      drawStringRGBA(text, textLen, x, y);
  }
}

//--------------------------------------------------------------
// draw a string
void mgGenContext::drawStringRGBA(
  const char* text,
  int textLen,
  int x,
  int y)
{
  DWORD pixel = (m_textColor.m_b) |
                (m_textColor.m_g << 8) |
                (m_textColor.m_r << 16);

  x += m_origin.m_x;
  y += m_origin.m_y;

  // get clip bounds
  int clipLeft = m_clipBounds.m_x;
  int clipTop = m_clipBounds.m_y;
  int clipRight = m_clipBounds.right();
  int clipBottom = m_clipBounds.bottom();

  int textPosn = 0;
  while (textPosn < textLen)
  {
    int utfCount;
    int letter = mgString::fromUTF8(text+textPosn, utfCount);
    textPosn += utfCount;

    const mgGenCharDefn* defn = m_surface->m_charCache->getChar(m_font->m_ftFace, letter);
    if (defn == NULL)
      continue;

    int outx = x + defn->m_bitmapX;
    int outy = y - defn->m_bitmapY;

    // advance to next char
    x += defn->m_advanceX;
    y += defn->m_advanceY;

    // test against clip bounds
    int outRight = outx + defn->m_bitmapWidth;
    int outBottom = outy + defn->m_bitmapHeight;

    int left = max(outx, clipLeft);
    int top = max(outy, clipTop);
    int right = min(outRight, clipRight);
    int bottom = min(outBottom, clipBottom);
    if (left >= right || top >= bottom)
      continue;

    // draw the character onto the surface
    DWORD* surfaceLine = m_surface->m_imageData + m_surface->m_imageWidth * top;
    BYTE* bitmapLine = defn->m_bitmapData + (top-outy)*defn->m_bitmapWidth;
    for (int cy = top; cy < bottom; cy++)
    {
      for (int cx = left; cx < right; cx++)
      {
        int alpha = bitmapLine[cx-outx];
        if (alpha != 0)
        {
          int outa = (m_alpha * alpha)/255;
          surfaceLine[cx] = pixel | (outa << 24);
        }
      }

      // next line on source and target
      bitmapLine += defn->m_bitmapWidth;
      surfaceLine += m_surface->m_imageWidth;
    }
  }
}

//--------------------------------------------------------------
// draw a string in RGB, merging text alpha with existing alpha
void mgGenContext::drawStringRGB(
  const char* text,
  int textLen,
  int x,
  int y)
{
  x += m_origin.m_x;
  y += m_origin.m_y;

  // get clip bounds
  int clipLeft = m_clipBounds.m_x;
  int clipTop = m_clipBounds.m_y;
  int clipRight = m_clipBounds.right();
  int clipBottom = m_clipBounds.bottom();

  int textPosn = 0;
  while (textPosn < textLen)
  {
    int utfCount;
    int letter = mgString::fromUTF8(text+textPosn, utfCount);
    textPosn += utfCount;

    const mgGenCharDefn* defn = m_surface->m_charCache->getChar(m_font->m_ftFace, letter);
    if (defn == NULL)
      continue;

    int outx = x + defn->m_bitmapX;
    int outy = y - defn->m_bitmapY;

    // advance to next char
    x += defn->m_advanceX;
    y += defn->m_advanceY;

    // test against clip bounds
    int outRight = outx + defn->m_bitmapWidth;
    int outBottom = outy + defn->m_bitmapHeight;

    int left = max(outx, clipLeft);
    int top = max(outy, clipTop);
    int right = min(outRight, clipRight);
    int bottom = min(outBottom, clipBottom);
    if (left >= right || top >= bottom)
      continue;

    // draw the character onto the surface
    DWORD* surfaceLine = m_surface->m_imageData + m_surface->m_imageWidth * top;
    BYTE* bitmapLine = defn->m_bitmapData + (top-outy)*defn->m_bitmapWidth;
    for (int cy = top; cy < bottom; cy++)
    {
      for (int cx = left; cx < right; cx++)
      {
        int alpha = bitmapLine[cx-outx];
        if (alpha != 0)
        {
          DWORD old = surfaceLine[cx];

          // combine text rgba with existin rgba, using char alpha to blend
          int oldb = old & 0xFF;
          int oldg = (old >> 8) & 0xFF;
          int oldr = (old >> 16) & 0xFF;
          int olda = (old >> 24) & 0xFF;
          int r = (m_textColor.m_r * alpha + oldr * (255-alpha))/255;
          int g = (m_textColor.m_g * alpha + oldg * (255-alpha))/255;
          int b = (m_textColor.m_b * alpha + oldb * (255-alpha))/255;
          int a = (m_alpha         * alpha + olda * (255-alpha))/255;
          surfaceLine[cx] = b | (g << 8) | (r << 16) | (a << 24);
        }
      }

      // next line on source and target
      bitmapLine += defn->m_bitmapWidth;
      surfaceLine += m_surface->m_imageWidth;
    }
  }
}

//--------------------------------------------------------------
// draw a string in alpha plane only
void mgGenContext::drawStringAlpha(
  const char* text,
  int textLen,
  int x,
  int y)
{
  x += m_origin.m_x;
  y += m_origin.m_y;

  // get clip bounds
  int clipLeft = m_clipBounds.m_x;
  int clipTop = m_clipBounds.m_y;
  int clipRight = m_clipBounds.right();
  int clipBottom = m_clipBounds.bottom();

  int textPosn = 0;
  while (textPosn < textLen)
  {
    int utfCount;
    int letter = mgString::fromUTF8(text+textPosn, utfCount);
    textPosn += utfCount;

    const mgGenCharDefn* defn = m_surface->m_charCache->getChar(m_font->m_ftFace, letter);
    if (defn == NULL)
      continue;

    int outx = x + defn->m_bitmapX;
    int outy = y - defn->m_bitmapY;

    // advance to next char
    x += defn->m_advanceX;
    y += defn->m_advanceY;

    // test against clip bounds
    int outRight = outx + defn->m_bitmapWidth;
    int outBottom = outy + defn->m_bitmapHeight;

    int left = max(outx, clipLeft);
    int top = max(outy, clipTop);
    int right = min(outRight, clipRight);
    int bottom = min(outBottom, clipBottom);
    if (left >= right || top >= bottom)
      continue;

    // draw the character onto the surface
    DWORD* surfaceLine = m_surface->m_imageData + m_surface->m_imageWidth * top;
    BYTE* bitmapLine = defn->m_bitmapData + (top-outy)*defn->m_bitmapWidth;
    for (int cy = top; cy < bottom; cy++)
    {
      for (int cx = left; cx < right; cx++)
      {
        int alpha = bitmapLine[cx-outx];
        if (alpha != 0)
        {
          alpha = (alpha*m_alpha)/255;
          surfaceLine[cx] = (surfaceLine[cx] & 0x00FFFFFF) | (alpha << 24);
        }
      }

      // next line on source and target
      bitmapLine += defn->m_bitmapWidth;
      surfaceLine += m_surface->m_imageWidth;
    }
  }
}

