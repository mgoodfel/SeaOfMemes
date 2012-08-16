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

#include "Graphics3D/mgTextureSurface.h"

const int SURFACE_TILE_SIZE = 256;

//--------------------------------------------------------------
// constructor
mgTextureSurface::mgTextureSurface(
  BOOL singleTile,
  BOOL inworld)
{
  m_singleTile = singleTile;
  m_inworld = inworld;

  m_tiles = NULL;
  m_vertTiles = 0;
  m_horzTiles = 0;
}

//--------------------------------------------------------------
// destructor
mgTextureSurface::~mgTextureSurface()
{
  deleteTiles();
}

//--------------------------------------------------------------
// get texture for normal surfaces (not multi-tiled)
mgTextureImage* mgTextureSurface::getTexture() const
{
  return m_tiles[0];
}

//--------------------------------------------------------------------
// repair damage
void mgTextureSurface::repair(
  mgRectangle& bounds)
{
  if (isDamaged())
  {
    mgGenSurface::repair(bounds);
    if (m_singleTile)
      updateSingleImage(bounds.m_x, bounds.m_y, bounds.right(), bounds.bottom());
    else updateTiledImage(bounds.m_x, bounds.m_y, bounds.right(), bounds.bottom());
  }
}

//--------------------------------------------------------------
// set dimensions of surface
void mgTextureSurface::setSurfaceSize(
  int width,
  int height)
{
  mgGenSurface::setSurfaceSize(width, height);

  if (m_singleTile)
    resizeSingleImage(width, height);
  else resizeTiledImage(width, height);
}

//--------------------------------------------------------------
// set dimensions of image
void mgTextureSurface::resizeSingleImage(
  int width,
  int height)
{
  deleteTiles();

  m_horzTiles = 1;
  m_vertTiles = 1;

  m_tiles = new mgTextureImage*[1];
  m_tiles[0] = mgDisplay->createTextureMemory(width, height,
                      MG_MEMORY_FORMAT_BGRA, m_inworld);
}

//--------------------------------------------------------------
// resize a tiled image
void mgTextureSurface::resizeTiledImage(
  int width,
  int height)
{
  // figure number of tiles required
  int newHorzTiles = (width+SURFACE_TILE_SIZE-1)/SURFACE_TILE_SIZE;
  int newVertTiles = (height+SURFACE_TILE_SIZE-1)/SURFACE_TILE_SIZE;
  
  // if same number of tiles
  if (m_horzTiles * m_vertTiles == newHorzTiles * newVertTiles)
  {
    // adjust dimensions of tile array
    m_horzTiles = newHorzTiles;
    m_vertTiles = newVertTiles;
    return;
  }

  int tileCount = newHorzTiles*newVertTiles;
  int oldTileCount = m_horzTiles * m_vertTiles;

  // reallocate the tile array
  mgTextureImage** newTiles = new mgTextureImage*[tileCount];
  
  // copy any old tiles we can reuse
  for (int i = 0; i < min(tileCount, oldTileCount); i++)
  {
    newTiles[i] = m_tiles[i];
  }
  
  // initialize additional new tiles
  DWORD* zeros = new DWORD[SURFACE_TILE_SIZE * SURFACE_TILE_SIZE];
  memset(zeros, 0, sizeof(DWORD) * SURFACE_TILE_SIZE*SURFACE_TILE_SIZE);
  for (int i = oldTileCount; i < tileCount; i++)
  {
    newTiles[i] = mgDisplay->createTextureMemory(SURFACE_TILE_SIZE, SURFACE_TILE_SIZE,
                      MG_MEMORY_FORMAT_BGRA, m_inworld);
    newTiles[i]->updateMemory(0, 0, SURFACE_TILE_SIZE, SURFACE_TILE_SIZE, (const BYTE*) zeros);
  }
  delete zeros; zeros = NULL;
    
  // delete unused old tiles
  for (int i = tileCount; i < oldTileCount; i++)
  {
    delete m_tiles[i];
  }
  
  // switch to new tiles
  delete m_tiles;
  m_tiles = NULL;

  m_tiles = newTiles;
  m_horzTiles = newHorzTiles;
  m_vertTiles = newVertTiles;
}

//--------------------------------------------------------------
// update single-texture image
void mgTextureSurface::updateSingleImage(
  int left,
  int top,
  int right,
  int bottom)
{
  // if updating entire tile
  if (left == 0 && top == 0 && right == m_imageWidth && bottom == m_imageHeight)
    m_tiles[0]->updateMemory(0, 0, m_imageWidth, m_imageHeight, (const BYTE*) m_imageData);

  else 
  {
    // extract damaged rectangle so we can call updateMemory
    int copyWidth = right-left;
    int copyHeight = bottom-top;
    DWORD* data = new DWORD[copyWidth*copyHeight];

    // extract the damaged area
    DWORD* dataLine = data;
    for (int y = top; y < bottom; y++)
    {
      DWORD* imageLine = m_imageData + m_imageWidth*y + left;
      memcpy(dataLine, imageLine, copyWidth*sizeof(DWORD));
      dataLine += copyWidth;
    }
    m_tiles[0]->updateMemory(left, top, copyWidth, copyHeight, (const BYTE*) data);
    delete data;
  }
}

//--------------------------------------------------------------
// update tiled image
void mgTextureSurface::updateTiledImage(
  int left,
  int top,
  int right,
  int bottom)
{
  // allocate tile-sized buffer
  DWORD* data = new DWORD[SURFACE_TILE_SIZE * SURFACE_TILE_SIZE];

  // update the modified tiles
  for (int i = 0; i < m_vertTiles; i++)
  {
    int ly = i*SURFACE_TILE_SIZE;
    int ht = min(SURFACE_TILE_SIZE, m_imageHeight - ly);
    int hy = ly+ht;

    for (int j = 0; j < m_horzTiles; j++)
    {
      int lx = j*SURFACE_TILE_SIZE;
      int wd = min(SURFACE_TILE_SIZE, m_imageWidth - lx);
      int hx = lx+wd;

      int copyLeft = max(left, lx);
      int copyRight = min(right, hx);
      int copyTop = max(top, ly);
      int copyBottom = min(bottom, hy);

      // if doesn't intersect the damage rectangle, no need to update
      if (copyRight < copyLeft || copyBottom < copyTop)
        continue;
    
      // copy the surface data to the buffer
      int copyWidth = copyRight - copyLeft;
      int copyHeight = copyBottom - copyTop;
      DWORD* dataLine = data;
      for (int y = copyTop; y < copyBottom; y++)
      {
        DWORD* imageLine = m_imageData + m_imageWidth * y + copyLeft;
        memcpy(dataLine, imageLine, copyWidth*sizeof(DWORD));
        dataLine += copyWidth;
      }

/*
      // =-= show bounds of update
      for (int y = 0; y < copyHeight; y++)
      {
        data[y*copyWidth+0] = 0xFF0000FF;
        data[y*copyWidth+copyWidth-1] = 0xFF0000FF;
      }
      for (int x = 0; x < copyWidth; x++)
      {
        data[0*copyWidth+x] = 0xFF0000FF;
        data[(copyHeight-1)*copyWidth+x] = 0xFF0000FF;
      }
*/
      // update the texture memory for the tile
      m_tiles[i*m_horzTiles+j]->updateMemory(copyLeft-lx, copyTop-ly, copyWidth, copyHeight, (const BYTE*) data);
    }
  }

  delete data;
}

//--------------------------------------------------------------------
// create buffers, ready to send to display
void mgTextureSurface::createBuffers()
{
  if (m_tiles == NULL)
    return;
    
  if (m_singleTile)
  {
    if (m_tiles[0] == NULL)
      m_tiles[0] = mgDisplay->createTextureMemory(m_imageWidth, m_imageHeight, MG_MEMORY_FORMAT_BGRA, m_inworld);
  }
  else
  {
    for (int i = 0; i < m_vertTiles * m_horzTiles; i++)
    {
      if (m_tiles[i] == NULL)
        m_tiles[i] = mgDisplay->createTextureMemory(SURFACE_TILE_SIZE, SURFACE_TILE_SIZE, MG_MEMORY_FORMAT_BGRA, m_inworld);
    }
  }

  // recreate tiles from Bitmaps
  if (m_singleTile)
    updateSingleImage(0, 0, m_imageWidth, m_imageHeight);
  else updateTiledImage(0, 0, m_imageWidth, m_imageHeight);
}

//--------------------------------------------------------------------
// draw the surface as an overlay
void mgTextureSurface::drawOverlay(
  int x,
  int y) const
{
  if (m_singleTile)
  {
    mgDisplay->drawOverlayTexture(m_tiles[0], x, y, m_imageWidth, m_imageHeight);
    return;
  }

  // handle multi-tile case
  for (int i = 0; i < m_vertTiles; i++)
  {
    int ty = SURFACE_TILE_SIZE * i;
    for (int j = 0; j < m_horzTiles; j++)
    {
      int tx = SURFACE_TILE_SIZE * j;
      mgTextureImage* tile = m_tiles[i*m_horzTiles+j];
      mgDisplay->drawOverlayTexture(tile, x+tx, y+ty, tile->m_width, tile->m_height);
    }
  }
}

//--------------------------------------------------------------------
// delete any display buffers
void mgTextureSurface::deleteBuffers()
{
  if (m_tiles == NULL)
    return;

  for (int i = 0; i < m_vertTiles * m_horzTiles; i++)
  {
    delete m_tiles[i];
    m_tiles[i] = NULL;
  }
}

//--------------------------------------------------------------
// delete all the tiles
void mgTextureSurface::deleteTiles()
{
  if (m_tiles == NULL)
    return;

  for (int i = 0; i < m_vertTiles * m_horzTiles; i++)
  {
    delete m_tiles[i];
    m_tiles[i] = NULL;
  }

  delete m_tiles;
  m_tiles = NULL;
}

//--------------------------------------------------------------
// return vertical pixels per inch
int mgTextureSurface::displayDPI() const
{
  return mgDisplay->getDPI();
}

//--------------------------------------------------------------
// find a font file
BOOL mgTextureSurface::findFont(
  const char* faceName, 
  BOOL bold, 
  BOOL italic, 
  mgString& fontFile)
{
  return mgDisplay->findFont(faceName, bold, italic, fontFile);
}
