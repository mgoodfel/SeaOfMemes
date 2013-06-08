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
#ifndef MGCONTEXT_H
#define MGCONTEXT_H

#include "mgTypes2D.h"
#include "mgSurface.h"

const int MG_ALPHA_SET      = 1;
const int MG_ALPHA_MERGE    = 2;
const int MG_ALPHA_RGB      = 3;

/*
  A drawing context which saves current state such as pen, brush,
  clipping bounds.  Methods support drawing text, lines and images.
  Very very crude at this point.
*/
class mgContext
{
public:
  // destructor
  virtual ~mgContext()
  {}

  // return underlying surface
  virtual mgSurface* getSurface() = 0;

  // reset drawing state
  virtual void resetState() = 0;
  
  // return current state
  virtual void* getState() = 0;
    
  // restore previous state
  virtual void setState(
    void* state) = 0;

  // delete a saved state
  virtual void deleteState(
    void* state) = 0;
    
  // set clipping rectangle
  virtual void setClip(
    int x,
    int y, 
    int width,
    int height) = 0;

  // translate coordinate system
  virtual void translate(
    double x,
    double y) = 0;

#ifdef WORKED
  // rotate coordinate space
  virtual void rotate(
    double radians) = 0;
#endif

  // get clipping bounds
  virtual void getClip(
    mgRectangle& bounds) = 0;
      
  // set alpha update mode
  virtual void setAlphaMode(
    int mode) = 0;
  
  // set pen for lines
  virtual void setPen(
    const mgPen* pen) = 0;
    
  // set brush for fill
  virtual void setBrush(
    const mgBrush* brush) = 0;
    
  // set font for text
  virtual void setFont(
    const mgFont* font) = 0;
    
  // set color for text
  virtual void setTextColor(
    const mgColor& color) = 0;

  // draw line segment
  virtual void drawLine(
    double fx, 
    double fy, 
    double tx, 
    double ty) = 0;
    
  // draw rectangle
  virtual void drawRect(
    double x, 
    double y, 
    double width, 
    double height) = 0;
    
  // fill rectangle
  virtual void fillRect(
    double x,
    double y,
    double width,
    double height) = 0;

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
    double sht) = 0;
  
  // draw an image
  virtual void drawImage(
    const mgImage* image,
    double x,                // destination top-left
    double y) = 0;
  
  // draw an image
  virtual void drawImage(
    const mgImage* image,
    double x,                // destination top-left
    double y,
    double width,            // size
    double height) = 0;
  
  // draw an image scaled to fit rectangle
  virtual void drawImageWithin(
    const mgImage* image,
    double x,
    double y,
    double width,
    double height) = 0;

  // draw an icon
  virtual void drawIcon(
    const mgIcon* icon,
    double dx,               // destination top-left
    double dy,
    double dwd,              // destination size
    double dht,
    double sx,               // source top-left
    double sy,
    double swd,              // source size
    double sht) = 0;
  
  // draw an icon
  virtual void drawIcon(
    const mgIcon* icon,
    double x,                // destination top-left
    double y) = 0;
  
  // draw an icon
  virtual void drawIcon(
    const mgIcon* icon,
    double x,                // destination top-left
    double y,
    double width,            // size
    double height) = 0;
  
  // draw an icon scaled to fit rectangle
  virtual void drawIconWithin(
    const mgIcon* icon,
    double x,
    double y,
    double width,
    double height) = 0;

  // draw a string
  virtual void drawString(
    const char* text,
    int textLen,
    double x,
    double y) = 0;

  // output graphics
  virtual void flush() = 0;
};

#endif
