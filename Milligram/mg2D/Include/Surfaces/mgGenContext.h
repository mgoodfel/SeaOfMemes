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
#ifndef MGGENCONTEXT_H
#define MGGENCONTEXT_H

#include "Surfaces/mgGenSurface.h"

class mgGenFont;

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
    double x,
    double y);

#ifdef WORKED
  // rotate coordinate space
  virtual void rotate(
    double radians);
#endif

  // get clipping bounds
  virtual void getClip(
    mgRectangle& bounds);
      
  // set alpha drawing mode
  virtual void setAlphaMode(
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

  // draw line segment
  virtual void drawLine(
    double fx, 
    double fy, 
    double tx, 
    double ty);
    
  // draw rectangle
  virtual void drawRect(
    double x, 
    double y, 
    double width, 
    double height);
    
  // fill rectangle
  virtual void fillRect(
    double x,
    double y,
    double width,
    double height);

  // draw an image
  virtual void drawImage(
    const mgImage* image,
    double dx,               // destination top-left
    double dy,
    double dwd,              // destination size
    double dht,
    double sx,               // source top-left
    double sy,
    double swd,              // source size
    double sht);
  
  // draw an image
  virtual void drawImage(
    const mgImage* image,
    double x,                // destination top-left
    double y);
  
  // draw an image
  virtual void drawImage(
    const mgImage* image,
    double x,                // destination top-left
    double y,
    double width,            // size
    double height);
  
  // draw an image scaled to fit rectangle
  virtual void drawImageWithin(
    const mgImage* image,
    double x,
    double y,
    double width,
    double height);

  // draw an image
  virtual void drawIcon(
    const mgIcon* icon,
    double dx,               // destination top-left
    double dy,
    double dwd,              // destination size
    double dht,
    double sx,               // source top-left
    double sy,
    double swd,              // source size
    double sht);
  
  // draw an icon
  virtual void drawIcon(
    const mgIcon* icon,
    double x,                // destination top-left
    double y);
  
  // draw an icon
  virtual void drawIcon(
    const mgIcon* icon,
    double x,                // destination top-left
    double y,
    double width,            // size
    double height);
  
  // draw an icon scaled to fit rectangle
  virtual void drawIconWithin(
    const mgIcon* icon,
    double x,
    double y,
    double width,
    double height);

  // draw a string
  virtual void drawString(
    const char* text,
    int textLen,
    double x,
    double y);
  
  // output graphics
  virtual void flush();

protected:
  mgGenSurface* m_surface;

  mgGenContextState* m_state;  
  
  const mgGenFont* m_defaultFont;
  const mgPen* m_defaultPen;
  const mgBrush* m_defaultBrush;

  mgRectangle m_clipBounds;  // clip interesected with surface area

  // constructor
  mgGenContext(
    mgGenSurface* surface);

  friend class mgGenSurface;
};

#endif
