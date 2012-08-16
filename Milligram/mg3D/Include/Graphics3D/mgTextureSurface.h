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
#ifndef MGTEXTURESURFACE_H
#define MGTEXTURESURFACE_H

#include "mg2D/Include/Graphics2D/Surfaces/mgGenSurface.h"

class mgTextureImage;

// interface to texture for a surface
class mgTextureSurface : public mgGenSurface
{
public:
  // constructor
  mgTextureSurface(
    BOOL singleTile,
    BOOL inworld);

  // destructor
  virtual ~mgTextureSurface();

  // return texture containing surface data
  virtual mgTextureImage* getTexture() const;

  // create buffers, ready to send to display
  virtual void createBuffers();
  
  // delete any display buffers
  virtual void deleteBuffers();
  
  // draw the surface as an overlay
  virtual void drawOverlay(
    int x,
    int y) const;

  // repair damage
  virtual void repair(
    mgRectangle& bounds);

  // set dimensions of surface
  virtual void setSurfaceSize(
    int width,
    int height);

protected:
  BOOL m_singleTile;              // true to keep as single tile
  BOOL m_inworld;                 // true if world object, false=headspace
  int m_horzTiles;                // count of horizontal tiles
  int m_vertTiles;                // count of vertical tiles
  mgTextureImage** m_tiles;       // texture array

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

  // update single texture image
  virtual void updateSingleImage(
    int left,
    int top,
    int right,
    int bottom);

  // update tiled image
  virtual void updateTiledImage(
    int left,
    int top,
    int right,
    int bottom);

  // return vertical pixels per inch
  virtual int displayDPI() const;

  // find a font file
  virtual BOOL findFont(
    const char* faceName, 
    BOOL bold, 
    BOOL italic, 
    mgString& fontFile);
};

#endif

