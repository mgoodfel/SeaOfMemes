/*
  Copyright (C) 1995-2011 by Michael J. Goodfellow

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
#if !defined(MGXPSURFACE_H) && defined(SUPPORT_WINXP_GRAPHICS)
#define MGXPSURFACE_H

#include "Graphics2D/mgGraphics2D.h"

const int SURFACE_TILE_SIZE = 256;

class mgTextureImage;
class mgDisplayServices;
class mgXPContext;
class mgXPFont;
class mgWinSystem;

// Windows XP implementation of the mgSurface
class mgXPSurface : public mgSurface
{
public:
  // constructor
  mgXPSurface();

  // destructor
  virtual ~mgXPSurface();

  // create a drawing context
  virtual mgContext* getContext();

  // get texture for normal surfaces (not multi-tiled)
  virtual mgTextureImage* getTexture();

  // convert points to pixels on this device
  virtual int points(
    double ptSize);
      
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

  // create an image
  virtual const mgImage* createImage(
    const char* fileName);

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
    mgRectangle& bounds);
  
  virtual BOOL isDamaged()
  {
    return m_isDamaged;
  } 

  // repair damage
  virtual void repair();

  // delete any display buffers
  virtual void deleteBuffers();
  
  // create buffers, ready to send to display
  virtual void createBuffers();
  
  // set dimensions of surface
  virtual void setSurfaceSize(
    int width,
    int height);

  // draw the surface as an overlay
  virtual void drawOverlay();

protected:
  int m_logPixelsY;

  HDC m_surfaceDC;                // display context for surface image
  HBITMAP m_surfaceBitmap;        // surface image bitmap
  BYTE* m_surfaceData;            // surface image data

  HDC m_alphaDC;                  // display context for alpha image
  HBITMAP m_alphaBitmap;          // alpha image bitmap
  BYTE* m_alphaData;              // alpha image data

  BOOL m_singleTile;              // true to keep as single tile
  BOOL m_inworld;                 // true if world object, false=headspace
  int m_horzTiles;                // count of horizontal tiles
  int m_vertTiles;                // count of vertical tiles
  mgTextureImage** m_tiles;         // texture array

  BOOL m_isDamaged;

  int m_damageLeft;
  int m_damageTop;
  int m_damageRight;
  int m_damageBottom;

  mgMapStringToPtr m_fontCache;
  mgMapStringToPtr m_brushCache;
  mgMapStringToPtr m_penCache;

  // constructor
  mgXPSurface(
    BOOL singleTile,
    BOOL inworld=false);

  // common init
  virtual void init(
    BOOL singleTile,
    BOOL inworld);

  // set dimensions of singletile image
  virtual void resizeSingleImage(
    int width,
    int height);

  // set dimensions of singletile image
  virtual void resizeTiledImage(
    int width,
    int height);

  // delete all the tiles
  virtual void deleteTiles();

  // update texture tiles with new image
  virtual void updateTiles(
    int left,
    int top,
    int right,
    int bottom);

  // write data of a tile
  virtual void writeTile(
    mgTextureImage* texture,
    int copyX,
    int copyY,
    int copyWidth,
    int copyHeight);

  // delete the cached fonts
  virtual void deleteFonts();

  // delete cached brushes
  virtual void deleteBrushes();

  // delete cached pens
  virtual void deletePens();

  friend mgXPContext;
  friend mgXPFont;
  friend mgWinSystem;
};

#endif
