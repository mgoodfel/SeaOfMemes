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

/*
  The generic surface renders to a memory array in the CPU with no use of 
  platform or GPU graphics capabilities.  Should work everywhere.
*/

#ifndef MGGENSURFACE_H
#define MGGENSURFACE_H

#pragma comment(lib, "FreeType.lib")

class mgGenContext;
class mgGenFont;
class mgGenCharCache;

// Generic platform implementation of the mgSurface
class mgGenSurface : public mgSurface
{
public:
  // constructor
  mgGenSurface();
  
  // destructor
  virtual ~mgGenSurface();

  // get surface pixels, 32-bits per pixel (DWORD=RGBA)
  virtual void getPixels(
    int& imageWidth,
    int& imageHeight,
    DWORD*& imageData) const;

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
    int r, 
    int g, 
    int b);
    
  // create a brush
  virtual const mgBrush* createBrush(
    const mgColor& color);
    
  // create a brush
  virtual const mgBrush* createBrush(
    const char* colorSpec);

  // create pen from color
  virtual const mgPen* createPen(
    int r, 
    int g, 
    int b,
    int thick);
    
  // create named color pen
  virtual const mgPen* createPen(
    const char* colorSpec,
    int thick);

  // create pen from color
  virtual const mgPen* createPen(
    const mgColor& color,
    int thick);

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

  // Applications must define this method to return vertical pixels per inch
  virtual int displayDPI() const = 0;

  // Applications must define this method to find a font file
  virtual BOOL findFont(
    const char* faceName, 
    BOOL bold, 
    BOOL italic, 
    mgString& fontFile) = 0;

protected:
  void* m_ftLibrary;
  mgGenCharCache* m_charCache;

  int m_imageWidth;
  int m_imageHeight;
  DWORD* m_imageData;            

  BOOL m_isDamaged;

  int m_damageLeft;
  int m_damageTop;
  int m_damageRight;
  int m_damageBottom;

  mgMapStringToPtr m_resourceCache;
  
  friend class mgGenContext;
  friend class mgGenFont;
};


#endif
