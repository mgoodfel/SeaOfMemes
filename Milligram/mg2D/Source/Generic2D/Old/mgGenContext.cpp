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
#ifdef SUPPORT_GEN2D

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgColor.h"
#include "mgGenPen.h"
#include "mgGenBrush.h"
#include "mgGenFont.h"
#include "mgGenImage.h"
#include "mgGenIcon.h"
#include "mgGenCharCache.h"
#include "Surfaces/mgGenSurface.h"

#include "mgGenContext.h"

class mgGenGraphicsState
{
public:
  mgRectangle m_reqClip;
//  XFORM m_transform;
  double m_originX;
  double m_originY;
  BOOL m_paintMode;
  int m_alphaMode;

  const mgGenFont* m_font;
  const mgGenBrush* m_brush;
  const mgGenPen* m_pen;
  mgColor m_textColor;
};

//--------------------------------------------------------------
// constructor
mgGenContext::mgGenContext(
  mgGenSurface* surface)
{
  m_surface = surface;

  m_defaultFont = (const mgGenFont*) m_surface->createFont("Arial", 10, false, false);
  m_defaultPen = (const mgGenPen*) m_surface->createPen(1.0, 0.0, 0.0, 0.0);
  m_defaultBrush = (const mgGenBrush*) m_surface->createBrush(0.0, 0.0, 0.0);
  
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
  m_alphaMode = MG_ALPHA_SET;

  m_font = m_defaultFont;
 
  m_pen = m_defaultPen;
  m_brush = m_defaultBrush;
  m_textColor = mgColor("black");

/*
  // set identity transform
  m_transform.eM11 = 1.0f;
  m_transform.eM12 = 0.0f; 
  m_transform.eM21 = 0.0f; 
  m_transform.eM22 = 1.0f; 
  m_transform.eDx  = 0.0f; 
  m_transform.eDy  = 0.0f;
*/
  m_originX = 0.0;
  m_originY = 0.0;

  // initialize clip
  m_reqClip.m_x = 0;
  m_reqClip.m_y = 0;
  m_reqClip.m_width = 32768;
  m_reqClip.m_height = 32768; 

  m_clipBounds.m_x = 0;
  m_clipBounds.m_y = 0;
  m_clipBounds.m_width = m_surface->m_surfaceWidth;
  m_clipBounds.m_height = m_surface->m_surfaceHeight;
}

//--------------------------------------------------------------
// return current state
void* mgGenContext::getState()
{
  mgGenGraphicsState* state = new mgGenGraphicsState();

//  state->m_transform = m_transform;
  state->m_originX = m_originX;
  state->m_originY = m_originY;
  state->m_reqClip = m_reqClip;
  state->m_paintMode = m_paintMode;
  state->m_alphaMode = m_alphaMode;

  state->m_brush = m_brush;
  state->m_pen = m_pen;
  state->m_font = m_font;
  state->m_textColor = m_textColor;

  return state;
}
  
//--------------------------------------------------------------
// restore previous state
void mgGenContext::setState(
  void* s)
{
  mgGenGraphicsState* state = (mgGenGraphicsState*) s;
  
//  m_transform = state->m_transform;
  m_originX = state->m_originX;
  m_originY = state->m_originY;

  // intersect requested clip bounds with surface size
  m_reqClip = state->m_reqClip;
  m_clipBounds.m_x = max(0, m_reqClip.m_x);
  m_clipBounds.m_y = max(0, m_reqClip.m_y);
  int clipRight = m_reqClip.m_x + m_reqClip.m_width;
  int clipBottom = m_reqClip.m_y + m_reqClip.m_height;
  m_clipBounds.m_width = min(m_surface->m_surfaceWidth, clipRight) - m_clipBounds.m_x;
  m_clipBounds.m_height = min(m_surface->m_surfaceHeight, clipBottom) - m_clipBounds.m_y;

  m_paintMode = state->m_paintMode;
  m_alphaMode = state->m_alphaMode;

  m_brush = state->m_brush;
  m_pen = state->m_pen;
  m_font = state->m_font;
  m_textColor = state->m_textColor;
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
void mgGenContext::setAlphaMode(
  int mode)
{
  m_alphaMode = mode;
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
  int left = max((int)m_originX + x, m_reqClip.m_x);
  int right = min((int)m_originX + x+width, m_reqClip.m_x+m_reqClip.m_width);
  int top = max((int)m_originY + y, m_reqClip.m_y);
  int bottom = min((int)m_originY + y+height, m_reqClip.m_y+m_reqClip.m_height);

  m_reqClip.m_x = left;
  m_reqClip.m_y = top;
  m_reqClip.m_width = max(0, right-left);
  m_reqClip.m_height = max(0, bottom-top);

  // intersect with surface bounds to get real clip rectangle
  m_clipBounds.m_x = max(0, left);
  m_clipBounds.m_y = max(0, top);
  m_clipBounds.m_width = min(m_surface->m_surfaceWidth, right) - m_clipBounds.m_x;
  m_clipBounds.m_height = min(m_surface->m_surfaceHeight, bottom) - m_clipBounds.m_y;
}

//--------------------------------------------------------------
// return box bounding current clip
void mgGenContext::getClip(
  mgRectangle& bounds)
{
  bounds.m_x = m_clipBounds.m_x - (int) m_originX;
  bounds.m_y = m_clipBounds.m_y - (int) m_originY;
  bounds.m_width = m_clipBounds.m_width;
  bounds.m_height = m_clipBounds.m_height;
}

//--------------------------------------------------------------
// translate coordinate system
void mgGenContext::translate(
  double x,
  double y)
{
  m_originX += x;
  m_originY += y;
}

#ifdef WORKED
//--------------------------------------------------------------------
// void GRContext.rotate(double)
void mgGenContext::rotate(
  double radians)
{
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
// draw line segment
void mgGenContext::drawLine(
  double fx, 
  double fy, 
  double tx, 
  double ty)
{
  fx += m_originX;
  fy += m_originY;
  tx += m_originX;
  ty += m_originY;

  // get pen thick/2 vector orthogonal to line
  double dx = tx-fx;
  double dy = ty-fy;
  double len = sqrt(dx*dx+dy*dy);
  double edgeX = (m_pen->m_thick * -dy)/(len*2);
  double edgeY = (m_pen->m_thick * dx)/(len*2);

#ifdef WORKED  
  // =-= fill two triangles
  mgPoint2* vertices = new mgPoint2[4];
  vertices[0].x = fx+edgeX;    vertices[0].y = fy+edgeY;
  vertices[1].x = tx+edgeX;    vertices[1].y = ty+edgeY;
  vertices[2].x = tx-edgeX;    vertices[2].y = ty-edgeY;
  vertices[3].x = fx-edgeX;    vertices[3].y = fy-edgeY;
#endif
}
  
//--------------------------------------------------------------
// draw rectangle
void mgGenContext::drawRect(
  double rectX, 
  double rectY, 
  double rectWidth, 
  double rectHeight)
{
  double left = rectX + m_originX;
  double right = left + rectWidth;
  double top = rectY + m_originY;
  double bottom = top + rectHeight;
  double thick = m_pen->m_thick;

  // =-= fill triangles to make rectangle
}
  
//--------------------------------------------------------------
// fill rectangle
void mgGenContext::fillRect(
  double rectX,
  double rectY,
  double rectWidth,
  double rectHeight)
{
  // clip the bounds
  rectX += m_originX;
  rectY += m_originY;

  double left = max(rectX, (double) m_clipBounds.m_x);
  double right = min(rectX+rectWidth, (double) m_clipBounds.m_x+m_clipBounds.m_width);

  double top = max(rectY, m_clipBounds.m_y);
  double bottom = min(rectY+rectHeight, (double) m_clipBounds.m_y+m_clipBounds.m_height);

  if (left >= right || top >= bottom)
    return;  // nothing to do

  // fill triangles
}

//--------------------------------------------------------------
// draw an image
void mgGenContext::drawImage(
  const mgImage* image,
  double dx,               // destination top-left
  double dy,
  double dwd,              // destination size
  double dht,
  double sx,               // source top-left
  double sy,
  double swd,              // source size
  double sht)
{
  mgGenImage* xpImage = (mgGenImage*) image;

  // clip the bounds
  dx += m_originX;
  dy += m_originY;

  int left = max(dx, m_clipBounds.m_x);
  int right = min(dx+dwd, m_clipBounds.m_x+m_clipBounds.m_width);

  int top = max(dy, m_clipBounds.m_y);
  int bottom = min(dy+dht, m_clipBounds.m_y+m_clipBounds.m_height);

  if (left >= right || top >= bottom)
    return;  // nothing to do

  // draw rectangle with image
}
        
//--------------------------------------------------------------
// draw an image
void mgGenContext::drawImage(
  const mgImage* image,
  double x,              // destination top-left
  double y)
{
  int imgWidth = image->getWidth();
  int imgHeight = image->getHeight();
  drawImage(image, x, y, imgWidth, imgHeight, 0, 0, imgWidth, imgHeight);
}
        
//--------------------------------------------------------------
// draw an image
void mgGenContext::drawImage(
  const mgImage* image,
  double x,              // destination top-left
  double y,
  double width,
  double height)
{
  int imgWidth = image->getWidth();
  int imgHeight = image->getHeight();
  drawImage(image, x, y, width, height, 0, 0, imgWidth, imgHeight);
}
        
//--------------------------------------------------------------
// draw an image scaled to fit rectangle
void mgGenContext::drawImageWithin(
  const mgImage* image,
  double x,
  double y,
  double width,
  double height)
{
  int imgWidth = image->getWidth();
  int imgHeight = image->getHeight();

  if (imgWidth == 0 || imgHeight == 0)
    return;
    
  double targetWidth = width;
  double targetHeight = height;
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

  double targetX = x+(width-targetWidth)/2;
  double targetY = y+(height-targetHeight)/2;

  drawImage(image, targetX, targetY, targetWidth, targetHeight, 0, 0, imgWidth, imgHeight);
}

//--------------------------------------------------------------
// draw an icon
void mgGenContext::drawIcon(
  const mgIcon* icon,
  double dx,               // destination top-left
  double dy,
  double dwd,              // destination size
  double dht,
  double sx,               // source top-left
  double sy,
  double swd,              // source size
  double sht)
{
  mgGenIcon* xpIcon = (mgGenIcon*) icon;

  // clip the bounds
  dx += m_originX;
  dy += m_originY;

  int left = max(dx, m_clipBounds.m_x);
  int right = min(dx+dwd, m_clipBounds.m_x+m_clipBounds.m_width);

  int top = max(dy, m_clipBounds.m_y);
  int bottom = min(dy+dht, m_clipBounds.m_y+m_clipBounds.m_height);

  if (left >= right || top >= bottom)
    return;  // nothing to do

  // draw rectangle with image 
}
        
//--------------------------------------------------------------
// draw an icon
void mgGenContext::drawIcon(
  const mgIcon* icon,
  double x,              // destination top-left
  double y)
{
  int iconWidth = icon->getWidth();
  int iconHeight = icon->getHeight();
  drawIcon(icon, x, y, iconWidth, iconHeight, 0, 0, iconWidth, iconHeight);
}
        
//--------------------------------------------------------------
// draw an icon
void mgGenContext::drawIcon(
  const mgIcon* icon,
  double x,              // destination top-left
  double y,
  double width,
  double height)
{
  int iconWidth = icon->getWidth();
  int iconHeight = icon->getHeight();
  drawIcon(icon, x, y, width, height, 0, 0, iconWidth, iconHeight);
}
        
//--------------------------------------------------------------
// draw an icon scaled to fit rectangle
void mgGenContext::drawIconWithin(
  const mgIcon* icon,
  double x,
  double y,
  double width,
  double height)
{
  int iconWidth = icon->getWidth();
  int iconHeight = icon->getHeight();

  if (iconWidth == 0 || iconHeight == 0)
    return;
    
  double targetWidth = width;
  double targetHeight = height;
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

  double targetX = x+(width-targetWidth)/2;
  double targetY = y+(height-targetHeight)/2;

  drawIcon(icon, targetX, targetY, targetWidth, targetHeight, 0, 0, iconWidth, iconHeight);
}

//--------------------------------------------------------------
// draw a string
void mgGenContext::drawString(
  const char* text,
  int textLen,
  double x,
  double y)
{
  // call surface to draw string with coordinates, color, alpha mode, clip
}

#endif
