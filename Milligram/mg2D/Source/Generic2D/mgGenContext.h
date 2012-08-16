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
#ifndef MGGENCONTEXT_H
#define MGGENCONTEXT_H

#include "mgGenFont.h"
#include "mgGenBrush.h"
#include "mgGenPen.h"
#include "Graphics2D/Surfaces/mgGenSurface.h"

// Windows implementation of a drawing context created on a WinSurface
class mgGenContext : public mgContext
{
public:
  // destructor
  virtual ~mgGenContext();

  // return underlying surface
  mgSurface* getSurface()
  {
    return m_surface;
  }

  // reset drawing state
  virtual void resetState();
  
  // return current state
  virtual void* getState();
    
  // restore previous state
  virtual void setState(
    void* state);

  // delete a saved state
  virtual void deleteState(
    void* state);
    
  // set clipping rectangle
  virtual void setClip(
    int x,
    int y, 
    int width,
    int height);

  // translate coordinate system
  virtual void translate(
    int x,
    int y);

#ifdef WORKED
  // rotate coordinate space
  virtual void rotate(
    double radians);
#endif

  // get clipping bounds
  virtual void getClip(
    mgRectangle& bounds);
      
  // set drawing mode
  virtual void setDrawMode(
    int mode);
  
  // set pen for lines
  virtual void setPen(
    const mgPen* pen);
    
  // set brush for fill
  virtual void setBrush(
    const mgBrush* brush);
    
  // set font for text
  virtual void setFont(
    const mgFont* font);
    
  // set color for text
  virtual void setTextColor(
    const mgColor& color);

  // set alpha plane value
  virtual void setAlpha(
    int alpha);
        
  // draw line segment
  virtual void drawLine(
    int fx, 
    int fy, 
    int tx, 
    int ty);
    
  // draw rectangle
  virtual void drawRect(
    int x, 
    int y, 
    int width, 
    int height);
    
  // fill rectangle
  virtual void fillRect(
    int x,
    int y,
    int width,
    int height);

  // draw an image
  virtual void drawImage(
    const mgImage* image,
    int dx,               // destination top-left
    int dy,
    int dwd,              // destination size
    int dht,
    int sx,               // source top-left
    int sy,
    int swd,              // source size
    int sht);
  
  // draw an image
  virtual void drawImage(
    const mgImage* image,
    int x,                // destination top-left
    int y);
  
  // draw an image
  virtual void drawImage(
    const mgImage* image,
    int x,                // destination top-left
    int y,
    int width,            // size
    int height);
  
  // draw an image scaled to fit rectangle
  virtual void drawImageWithin(
    const mgImage* image,
    int x,
    int y,
    int width,
    int height);

  // draw an image
  virtual void drawIcon(
    const mgIcon* icon,
    int dx,               // destination top-left
    int dy,
    int dwd,              // destination size
    int dht,
    int sx,               // source top-left
    int sy,
    int swd,              // source size
    int sht);
  
  // draw an icon
  virtual void drawIcon(
    const mgIcon* icon,
    int x,                // destination top-left
    int y);
  
  // draw an icon
  virtual void drawIcon(
    const mgIcon* icon,
    int x,                // destination top-left
    int y,
    int width,            // size
    int height);
  
  // draw an icon scaled to fit rectangle
  virtual void drawIconWithin(
    const mgIcon* icon,
    int x,
    int y,
    int width,
    int height);

  // draw a string
  virtual void drawString(
    const char* text,
    int textLen,
    int x,
    int y);
  
protected:
  mgGenSurface* m_surface;
  
//  XFORM m_transform;
  mgRectangle m_reqClip;  // requested clip
  mgRectangle m_clipBounds;  // clip interesected with surface area
  mgPoint m_origin;
  BOOL m_paintMode;
  const mgGenFont* m_font;
  const mgGenPen* m_pen;
  const mgGenBrush* m_brush;
  mgColor m_textColor;
  BYTE m_alpha;
  
  const mgGenFont* m_defaultFont;
  const mgGenPen* m_defaultPen;
  const mgGenBrush* m_defaultBrush;

  int m_drawMode;
    
  // constructor
  mgGenContext(
    mgGenSurface* surface);

  // fill rectangle in rgb planes.  
  // bounds already clipped, in surface coords
  void fillRectRGB(
    int rectLeft,
    int rectTop,
    int rectRight,
    int rectBottom,
    DWORD pixel);

  // fill rectangle in rgba planes.  
  // bounds already clipped, in surface coords
  void fillRectRGBA(
    int rectLeft,
    int rectTop,
    int rectRight,
    int rectBottom,
    DWORD pixel);

  // fill rectangle in rgb planes.  
  // bounds already clipped, in surface coords
  void fillRectAlpha(
    int rectLeft,
    int rectTop,
    int rectRight,
    int rectBottom,
    BYTE alpha);

  // draw a string in RGB, merging alpha
  void drawStringRGB(
    const char* text,
    int textLen,
    int x,
    int y);
  
  // draw a string only in alpha plane
  void drawStringAlpha(
    const char* text,
    int textLen,
    int x,
    int y);
  
  // draw a string in RGBA
  void drawStringRGBA(
    const char* text,
    int textLen,
    int x,
    int y);
  

  friend class mgGenSurface;
  friend class mgGenFont;
};

#endif
