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

#ifndef MGGENSURFACE_H
#define MGGENSURFACE_H

class mgGenContext;
class mgGenFont;
class mgGenIcon;
class mgGenImage;

class mgGenContextState
{
public:
  mgRectangle m_reqClip;
//  mgMatrix3 m_transform;
  double m_originX;
  double m_originY;
  BOOL m_paintMode;
  int m_alphaMode;

  const mgGenFont* m_font;
  const mgBrush* m_brush;
  const mgPen* m_pen;
  mgColor m_textColor;
};

/*
  A "generic" surface which implements the mgSurface interface
  on top of simpler primitives.
*/

class mgGenSurface : public mgSurface
{
public:
  int m_surfaceWidth;
  int m_surfaceHeight;

  // constructor
  mgGenSurface();
  
  // destructor
  virtual ~mgGenSurface();

  // create a drawing context
  virtual mgContext* newContext();

  // convert points to pixels on this device
  virtual int points(
    double ptSize) const;
      
  // create font
  virtual const mgFont* createFont(
    const char* faceName,
    int size,
    BOOL bold,
    BOOL italic);

  // create font from "arial-10-b-i" style fontspec
  virtual const mgFont* createFont(
    const char* fontSpec);

  // create a brush
  virtual const mgBrush* createBrush(
    const mgColor& color);
    
  // create a brush
  virtual const mgBrush* createBrush(
    double r, 
    double g, 
    double b,
    double a = 1.0);
    
  // create a brush
  virtual const mgBrush* createBrush(
    const char* colorSpec);

  // create pen from color
  virtual const mgPen* createPen(
    double thick,
    const mgColor& color);

  // create named color pen
  virtual const mgPen* createPen(
    double thick,
    const char* colorSpec);

  // create pen from color
  virtual const mgPen* createPen(
    double thick,
    double r, 
    double g, 
    double b,
    double a = 1.0);

  // create an icon
  virtual const mgIcon* createIcon(
    const char* fileName);

  // create an image
  virtual const mgImage* createImage(
    const char* fileName);

  // save a resource
  virtual void saveResource(
    const mgResource* data);

  // find a resource
  virtual mgResource* findResource(
    const char* name) const;

  // remove a resource
  virtual void removeResource(
    const mgResource* data);

  // damage entire bitmap
  virtual void damageAll();

  // damage a rectangle 
  virtual void damage(
    int x,
    int y,
    int width,
    int height);

  // get damage bounds
  virtual void getDamage(
    mgRectangle& bounds) const;
  
  // return true if damaged
  virtual BOOL isDamaged() const;

  // repair damage
  virtual void repair(
    mgRectangle& bounds);

  // set dimensions of surface
  virtual void setSurfaceSize(
    int width,
    int height);

  // get dimensions of surface
  virtual void getSurfaceSize(
    int& width,
    int& height) const;

  // output graphics
  virtual void flush() = 0;

protected:
  BOOL m_isDamaged;

  int m_damageLeft;
  int m_damageTop;
  int m_damageRight;
  int m_damageBottom;

  mgMapStringToPtr m_resourceCache;
  
  // Subclasses must define this method to return vertical pixels per inch
  virtual int displayDPI() const = 0;

  // load a new font
  virtual void loadFont(
    mgGenFont* font) = 0;

  // delete a font
  virtual void deleteFont(
    mgGenFont* font) = 0;

  // load image data, return size and handle
  virtual void* loadImage(
    mgGenImage* image) = 0;

  // delete image data
  virtual void deleteImage(
    mgGenImage* image) = 0;

  // load icon data, return size and handle
  virtual void* loadIcon(
    mgGenIcon* icon) = 0;

  // delete icon data
  virtual void deleteIcon(
    mgGenIcon* icon) = 0;

  // fill a triangle with color
  virtual void fillSolidTriangle(
    mgGenContextState* state,
    const mgColor& color,
    double ax,
    double ay,
    double bx,
    double by,
    double cx,
    double cy) = 0;

  // fill a triangle with image data
  virtual void fillImageTriangle(
    mgGenContextState* state,
    void* imageHandle,
    double ax,
    double ay,
    double bx,
    double by,
    double cx,
    double cy) = 0;

  // fill a rectangle with color
  virtual void fillSolidRectangle(
    mgGenContextState* state,
    const mgColor& color,
    double lx,
    double ly,
    double hx,
    double hy) = 0;

  // fill a rectangle with image data
  virtual void fillImageRectangle(
    mgGenContextState* state,
    void* imageHandle,
    double dx,
    double dy,
    double dwd,
    double dht,
    double sx,
    double sy,
    double swd,
    double sht) = 0;

  // draw text in font with color
  virtual void drawString(
    mgGenContextState* state,
    const char* text,
    int textLen,
    double x,
    double y) = 0;

  // measure a string
  virtual void stringExtent(
    const mgGenFont* font,
    const char* text,
    int textLen,
    mgPoint& endPt,
    mgRectangle& bounds) = 0;
    
  // measure a string (total advance)
  virtual int stringWidth(
    const mgGenFont* font,
    const char* text,
    int textLen) = 0;
    
  // return number of chars which fit in width
  virtual int stringFit(
    const mgGenFont* font,
    const char* text,
    int textLen,
    int width) = 0;

  // remove all resources
  virtual void removeAllResources();

  friend class mgGenContext;
  friend class mgGenFont;
  friend class mgGenImage;
  friend class mgGenIcon;
};


#endif
