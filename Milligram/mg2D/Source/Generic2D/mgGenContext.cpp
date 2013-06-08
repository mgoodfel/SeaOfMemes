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

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgColor.h"
#include "Surfaces/mgGenSurface.h"

#include "Surfaces/mgGenContext.h"
#include "Surfaces/mgGenImage.h"
#include "Surfaces/mgGenIcon.h"
#include "Surfaces/mgGenFont.h"

//--------------------------------------------------------------
// constructor
mgGenContext::mgGenContext(
  mgGenSurface* surface)
{
  m_surface = surface;
  m_state = new mgGenContextState();

  m_defaultFont = (const mgGenFont*) m_surface->createFont("Arial", 10, false, false);
  m_defaultPen = (const mgPen*) m_surface->createPen(1.0, 0.0, 0.0, 0.0, 1.0);
  m_defaultBrush = (const mgBrush*) m_surface->createBrush(0.0, 0.0, 0.0, 1.0);
     
  resetState();
}

//--------------------------------------------------------------
// destructor
mgGenContext::~mgGenContext()
{
  delete m_state;
  m_state = NULL;
}

//--------------------------------------------------------------
// reset drawing state
void mgGenContext::resetState()
{
  m_state->m_paintMode = true;
  m_state->m_alphaMode = MG_ALPHA_SET;

  m_state->m_font = m_defaultFont;
 
  m_state->m_pen = m_defaultPen;
  m_state->m_brush = m_defaultBrush;
  m_state->m_textColor = mgColor("black");

/*
  // set identity transform
  m_transform.eM11 = 1.0f;
  m_transform.eM12 = 0.0f; 
  m_transform.eM21 = 0.0f; 
  m_transform.eM22 = 1.0f; 
  m_transform.eDx  = 0.0f; 
  m_transform.eDy  = 0.0f;
*/
  m_state->m_originX = 0.0;
  m_state->m_originY = 0.0;

  // initialize clip
  m_state->m_reqClip.m_x = 0;
  m_state->m_reqClip.m_y = 0;
  m_state->m_reqClip.m_width = 32768;
  m_state->m_reqClip.m_height = 32768; 

  m_clipBounds.m_x = 0;
  m_clipBounds.m_y = 0;
  m_clipBounds.m_width = m_surface->m_surfaceWidth;
  m_clipBounds.m_height = m_surface->m_surfaceHeight;
}

//--------------------------------------------------------------
// return current state
void* mgGenContext::getState()
{
  mgGenContextState* state = new mgGenContextState();

//  state->m_transform = m_transform;
  state->m_originX = m_state->m_originX;
  state->m_originY = m_state->m_originY;
  state->m_reqClip = m_state->m_reqClip;
  state->m_paintMode = m_state->m_paintMode;
  state->m_alphaMode = m_state->m_alphaMode;

  state->m_brush = m_state->m_brush;
  state->m_pen = m_state->m_pen;
  state->m_font = m_state->m_font;
  state->m_textColor = m_state->m_textColor;

  return state;
}
  
//--------------------------------------------------------------
// restore previous state
void mgGenContext::setState(
  void* s)
{
  mgGenContextState* state = (mgGenContextState*) s;
  
//  m_transform = state->m_transform;
  m_state->m_originX = state->m_originX;
  m_state->m_originY = state->m_originY;

  // intersect requested clip bounds with surface size
  m_state->m_reqClip = state->m_reqClip;
  m_clipBounds.m_x = max(0, m_state->m_reqClip.m_x);
  m_clipBounds.m_y = max(0, m_state->m_reqClip.m_y);
  int clipRight = m_state->m_reqClip.m_x + m_state->m_reqClip.m_width;
  int clipBottom = m_state->m_reqClip.m_y + m_state->m_reqClip.m_height;
  m_clipBounds.m_width = min(m_surface->m_surfaceWidth, clipRight) - m_clipBounds.m_x;
  m_clipBounds.m_height = min(m_surface->m_surfaceHeight, clipBottom) - m_clipBounds.m_y;

  m_state->m_paintMode = state->m_paintMode;
  m_state->m_alphaMode = state->m_alphaMode;

  m_state->m_brush = state->m_brush;
  m_state->m_pen = state->m_pen;
  m_state->m_font = state->m_font;
  m_state->m_textColor = state->m_textColor;
}

//--------------------------------------------------------------
// delete saved state
void mgGenContext::deleteState(
  void* state)
{
  delete (mgGenContextState*) state;
}

//--------------------------------------------------------------
// set drawing mode
void mgGenContext::setAlphaMode(
  int mode)
{
  m_state->m_alphaMode = mode;
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
  int left = max((int)m_state->m_originX + x, m_state->m_reqClip.m_x);
  int right = min((int)m_state->m_originX + x+width, m_state->m_reqClip.m_x + m_state->m_reqClip.m_width);
  int top = max((int)m_state->m_originY + y, m_state->m_reqClip.m_y);
  int bottom = min((int)m_state->m_originY + y+height, m_state->m_reqClip.m_y + m_state->m_reqClip.m_height);

  m_state->m_reqClip.m_x = left;
  m_state->m_reqClip.m_y = top;
  m_state->m_reqClip.m_width = max(0, right-left);
  m_state->m_reqClip.m_height = max(0, bottom-top);

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
  bounds.m_x = m_clipBounds.m_x - (int) m_state->m_originX;
  bounds.m_y = m_clipBounds.m_y - (int) m_state->m_originY;
  bounds.m_width = m_clipBounds.m_width;
  bounds.m_height = m_clipBounds.m_height;
}

//--------------------------------------------------------------
// translate coordinate system
void mgGenContext::translate(
  double x,
  double y)
{
  m_state->m_originX += x;
  m_state->m_originY += y;
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
  m_state->m_pen = pen;
}
  
//--------------------------------------------------------------
// set brush for fill
void mgGenContext::setBrush(
  const mgBrush* brush)
{
  m_state->m_brush = brush;
}
  
//--------------------------------------------------------------
// set font for text
void mgGenContext::setFont(
  const mgFont* font)
{
  m_state->m_font = (mgGenFont*) font;
}
  
//--------------------------------------------------------------
// set color for text
void mgGenContext::setTextColor(
  const mgColor& color)
{
  m_state->m_textColor = color;
}

//--------------------------------------------------------------
// draw line segment
void mgGenContext::drawLine(
  double fx, 
  double fy, 
  double tx, 
  double ty)
{
  // figure segment half thickness long, orthogonal to line
  double dx = tx - fx;
  double dy = ty - fy;
  double len = sqrt(dx*dx + dy*dy);
  double thick = m_state->m_pen->m_thick/(len*2);
  double lx = -dy*thick;
  double ly = dx*thick;

  const mgColor* color = &m_state->m_pen->m_color;
  m_surface->fillSolidTriangle(m_state, *color, fx+lx, fy+ly, fx-lx, fy-ly, tx+lx, ty+ly);
  m_surface->fillSolidTriangle(m_state, *color, tx+lx, ty+ly, fx-lx, fy-ly, tx-lx, ty-ly);
}
  
//--------------------------------------------------------------
// draw rectangle
void mgGenContext::drawRect(
  double left, 
  double top, 
  double width, 
  double height)
{
  double right = left + width;
  double bottom = top + height;

  if (left >= right || top >= bottom)
    return;  // nothing to do

  double thick = m_state->m_pen->m_thick;
  double halfThick = thick/2;
  const mgColor* color = &m_state->m_pen->m_color;

/*
  // draw top edge of rect
  m_surface->fillSolidRectangle(m_state, *color, left-halfThick, top-halfThick, right+halfThick, top+halfThick);

  // draw bottom edge of rect
  m_surface->fillSolidRectangle(m_state, *color, left-halfThick, bottom-halfThick, right+halfThick, bottom+halfThick);

  // draw left edge of rect
  m_surface->fillSolidRectangle(m_state, *color, left-halfThick, top+halfThick, left+halfThick, bottom-halfThick);

  // draw right edge of rect
  m_surface->fillSolidRectangle(m_state, *color, right-halfThick, top+halfThick, right+halfThick, bottom-halfThick);
*/
  // above is correct if rect is lines with width centered on line.  GUI expects thickness in from edges.

  // draw top edge of rect
  m_surface->fillSolidRectangle(m_state, *color, left, top, right, top+thick);

  // draw bottom edge of rect
  m_surface->fillSolidRectangle(m_state, *color, left, bottom-thick, right, bottom);

  // draw left edge of rect
  m_surface->fillSolidRectangle(m_state, *color, left, top+thick, left+thick, bottom-thick);

  // draw right edge of rect
  m_surface->fillSolidRectangle(m_state, *color, right-thick, top+thick, right, bottom-thick);
}
  
//--------------------------------------------------------------
// fill rectangle
void mgGenContext::fillRect(
  double left,
  double top,
  double width,
  double height)
{
  if (width <= 0 || height <= 0)
    return;  // nothing to do

  // fill rectangle using surface
  m_surface->fillSolidRectangle(m_state, m_state->m_brush->m_color, left, top, left+width, top+height);
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
  mgGenImage* ourImage = (mgGenImage*) image;

  // fill rectangle with image
  m_surface->fillImageRectangle(m_state, ourImage->m_handle, dx, dy, dwd, dht, sx, sy, swd, sht);
}
        
//--------------------------------------------------------------
// draw an image
void mgGenContext::drawImage(
  const mgImage* image,
  double x,              // destination top-left
  double y)
{
  drawImage(image, x, y, image->m_width, image->m_height, 0, 0, image->m_width, image->m_height);
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
  drawImage(image, x, y, width, height, 0, 0, image->m_width, image->m_height);
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
  int imgWidth = image->m_width;
  int imgHeight = image->m_height;

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
  mgGenImage* ourIcon = (mgGenImage*) icon;

  // fill rectangle with image
  m_surface->fillImageRectangle(m_state, ourIcon->m_handle, dx, dy, dwd, dht, sx, sy, swd, sht);
}
        
//--------------------------------------------------------------
// draw an icon
void mgGenContext::drawIcon(
  const mgIcon* icon,
  double x,              // destination top-left
  double y)
{
  drawIcon(icon, x, y, icon->m_width, icon->m_height, 0, 0, icon->m_width, icon->m_height);
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
  drawIcon(icon, x, y, width, height, 0, 0, icon->m_width, icon->m_height);
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
  int iconWidth = icon->m_width;
  int iconHeight = icon->m_height;

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
  // call surface to draw string
  m_surface->drawString(m_state, text, textLen, x, y);
}

//--------------------------------------------------------------
// output graphics
void mgGenContext::flush()
{
  m_surface->flush();
}
