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
#ifdef SUPPORT_WINXP_GRAPHICS

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Graphics3D/mgDisplayServices.h"

#include "mgXPSurface.h"
#include "mgXPContext.h"
#include "mgXPImage.h"
#include "mgXPIcon.h"

//--------------------------------------------------------------
// public constructor only creates inworld-single tile images
mgXPSurface::mgXPSurface()
{
  init(true, true);
}

//--------------------------------------------------------------
// constructor
mgXPSurface::mgXPSurface(
  BOOL singleTile,
  BOOL inworld)
{
  init(singleTile, inworld);
}

//--------------------------------------------------------------
// common init
void mgXPSurface::init(
  BOOL singleTile,
  BOOL inworld)
{
  m_singleTile = singleTile;
  m_inworld = inworld;

  // create the DC so we can measure text before setting size
  m_surfaceDC = CreateCompatibleDC(NULL);
  m_alphaDC = CreateCompatibleDC(NULL);
  m_imageDC = CreateCompatibleDC(NULL);

  m_nullPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));

  SetGraphicsMode(m_surfaceDC, GM_ADVANCED);
  SetStretchBltMode(m_surfaceDC, COLORONCOLOR);
  SetBrushOrgEx(m_surfaceDC, 0, 0, NULL);
  SetBkMode(m_surfaceDC, TRANSPARENT);
  SetTextAlign(m_surfaceDC, TA_BASELINE | TA_LEFT);

  SetGraphicsMode(m_alphaDC, GM_ADVANCED);
  SetStretchBltMode(m_alphaDC, COLORONCOLOR);
  SetBrushOrgEx(m_alphaDC, 0, 0, NULL);
  SetBkMode(m_alphaDC, TRANSPARENT);
  SetTextAlign(m_alphaDC, TA_BASELINE | TA_LEFT);

  m_logPixelsY = GetDeviceCaps(m_surfaceDC, LOGPIXELSY);

  m_surfaceBitmap = NULL;
  m_alphaBitmap = NULL;
  m_isDamaged = false;

  m_imageWidth = 0;
  m_imageHeight = 0;

  m_currentGC = NULL;

  m_tiles = NULL;
  m_vertTiles = 0;
  m_horzTiles = 0;
}

//--------------------------------------------------------------
// destructor
mgXPSurface::~mgXPSurface()
{
  deleteTiles();

  // delete the surface bitmap
  if (m_surfaceBitmap != NULL)
  {
    DeleteObject(m_surfaceBitmap);
    m_surfaceBitmap = NULL;
  }

  // delete the surface DC
  SelectObject(m_surfaceDC, (HBITMAP) NULL);
  SelectObject(m_surfaceDC, (HFONT) NULL);
  SelectObject(m_surfaceDC, (HBRUSH) NULL);
  SelectObject(m_surfaceDC, (HPEN) NULL);
  DeleteDC(m_surfaceDC);
  m_surfaceDC = NULL;

  // delete the alpha bitmap
  if (m_alphaBitmap != NULL)
  {
    DeleteObject(m_alphaBitmap);
    m_alphaBitmap = NULL;
  }

  // delete the alpha DC
  SelectObject(m_alphaDC, (HBITMAP) NULL);
  SelectObject(m_alphaDC, (HFONT) NULL);
  SelectObject(m_alphaDC, (HBRUSH) NULL);
  SelectObject(m_alphaDC, (HPEN) NULL);
  DeleteDC(m_alphaDC);
  m_alphaDC = NULL;

  // delete the image DC
  DeleteDC(m_imageDC);
  m_imageDC = NULL;

  DeleteObject(m_nullPen);
  m_nullPen = NULL;

  // delete the cached drawing objects
  int posn = m_resourceCache.getStartPosition();
  while (posn != -1)
  {
    mgString key;
    const void* value;
    m_resourceCache.getNextAssoc(posn, key, value);
    delete (mgResource*) value;
  }
  m_resourceCache.removeAll();
}

//--------------------------------------------------------------
// get texture for normal surfaces (not multi-tiled)
mgTextureImage* mgXPSurface::getTexture()
{
  return m_tiles[0];
}

//--------------------------------------------------------------
// convert points to pixels on this device
int mgXPSurface::points(
  double ptSize)
{
  return (int) floor(0.5+(ptSize*m_logPixelsY)/72.0);
}      

//--------------------------------------------------------------
// create font
const mgFont* mgXPSurface::createFont(
  const char* faceName,
  int size,
  BOOL bold,
  BOOL italic)
{
  // construct name used for hashtable keys
  mgString cacheKey;
  cacheKey.format("font-%s-%d%s%s", (const char*) faceName, size, bold?"-B":"", italic?"-I":"");

  // if found, we're done
  mgResource* value = findResource(cacheKey);
  if (value != NULL)
    return (mgFont*) value;

  // create font and add to cache
  mgXPFont* font = new mgXPFont(this, faceName, size, bold, italic);
  saveResource(cacheKey, font);

  return font;
}

//--------------------------------------------------------------
// create a font from "arial-10-b-i" style fontspec
const mgFont* mgXPSurface::createFont(
  const char* spec)
{
  // parse fontSpec
  mgString faceName;
  int size = 12;
  BOOL bold = false;
  BOOL italic = false;
  
  mgString fontSpec(spec);
  // parse spec as facename-size-B-I
  int posn = fontSpec.find(0, "-");
  if (posn < fontSpec.length())
  {
    fontSpec.substring(faceName, 0, posn);
    posn = fontSpec.nextLetter(posn);
  }
  else faceName = spec;

  mgString digits;

  if (posn < fontSpec.length())
  {
    int dash2 = fontSpec.find(posn, "-");
    if (dash2 != -1)
    {
      fontSpec.substring(digits, posn, dash2-posn);
      posn = dash2;
    }
    else 
    {
      fontSpec.substring(digits, posn);
      posn = fontSpec.length();
    }
    if (1 != sscanf((const char*) digits, "%d", &size))
      throw new mgErrorMsg("2dBadFontSpec", "spec", "%s", (const char*) spec);
  }

  char letter[MG_MAX_LETTER];
  while (posn < fontSpec.length())
  {
    posn = fontSpec.nextLetter(posn, letter);
    if (strcmp(letter, "-") != 0)
      throw new mgErrorMsg("2dBadFontSpec", "spec", "%s", (const char*) spec);

    posn = fontSpec.nextLetter(posn, letter);
    if (_stricmp(letter, "B") == 0)
      bold = true;
    else if (_stricmp(letter, "I") == 0)
      italic = true;
    else throw new mgErrorMsg("2dBadFontSpec", "spec", "%s", (const char*) spec);
  }
  
  return createFont(faceName, size, bold, italic);
}

//--------------------------------------------------------------
// create a brush
const mgBrush* mgXPSurface::createBrush(
  int r, 
  int g, 
  int b)
{
  mgString cacheKey;
  cacheKey.format("brush-%d,%d,%d", r, g, b);
  
  mgResource* value = findResource(cacheKey);
  if (value != NULL)
    return (mgBrush*) value;

  mgXPBrush* brush = new mgXPBrush(r, g, b);
  saveResource(cacheKey, brush);

  return brush;
}
    
//--------------------------------------------------------------
// create a brush
const mgBrush* mgXPSurface::createBrush(
  const mgColor& color)
{
  return createBrush(color.m_r, color.m_g, color.m_b);
}
    
//--------------------------------------------------------------
// create a brush
const mgBrush* mgXPSurface::createBrush(
  const char* colorSpec)
{
  mgColor color(colorSpec);
  return createBrush(color.m_r, color.m_g, color.m_b);
}

//--------------------------------------------------------------
// create pen from color
const mgPen* mgXPSurface::createPen(
  int r, 
  int g, 
  int b,
  int thick)
{
  mgString cacheKey;
  cacheKey.format("pen-%d,%d,%d,%d", r, g, b, thick);
  
  mgResource* value = findResource(cacheKey);
  if (value != NULL)
    return (mgPen*) value;

  // create the pen
  mgXPPen* pen = new mgXPPen(r, g, b, thick);
  saveResource(cacheKey, pen);
  
  return pen;
}
    
//--------------------------------------------------------------
// create named color pen
const mgPen* mgXPSurface::createPen(
  const char* colorSpec,
  int thick)
{
  mgColor color(colorSpec);
  return createPen(color.m_r, color.m_g, color.m_b, thick);
}

//--------------------------------------------------------------
// create pen from color
const mgPen* mgXPSurface::createPen(
  const mgColor& color,
  int thick)
{
  return createPen(color.m_r, color.m_g, color.m_b, thick);
}

//--------------------------------------------------------------
// create an icon
const mgIcon* mgXPSurface::createIcon(
  const char* fileName)
{
  mgString cacheKey;
  cacheKey.format("icon-%s", (const char*) fileName);
  mgResource* value = findResource(cacheKey);
  if (value != NULL)
    return (mgIcon*) value;

  // create the icon
  mgXPIcon* icon = new mgXPIcon(m_surfaceDC, fileName);
  saveResource(fileName, icon);
  
  return icon;
}

//--------------------------------------------------------------
// create an image
const mgImage* mgXPSurface::createImage(
  const char* fileName)
{
  return new mgXPImage(m_surfaceDC, fileName);
}

//--------------------------------------------------------------
// save a resource
void mgXPSurface::saveResource(
  const char* name,
  const mgResource* data)
{
  m_resourceCache.setAt(name, data);
}

//--------------------------------------------------------------
// find a resource
mgResource* mgXPSurface::findResource(
  const char* name)
{
  const void* value;
  if (m_resourceCache.lookup(name, value))
    return (mgResource*) value;

  return NULL;
}

//--------------------------------------------------------------
// create a drawing context
mgContext* mgXPSurface::newContext()
{
  return (mgContext*) new mgXPContext(this);
}
  
//--------------------------------------------------------------------
// damage a rectangle 
void mgXPSurface::damage(
  int x,
  int y,
  int width,
  int height)
{
  // union this rect with damaged area, set isDamaged
  if (m_isDamaged)
  {
    m_damageLeft = max(0, min(m_damageLeft, x));
    m_damageTop = max(0, min(m_damageTop, y));
    m_damageRight = min(m_imageWidth, max(m_damageRight, x+width));
    m_damageBottom = min(m_imageHeight, max(m_damageBottom, y+height));
  }
  else
  {
    m_damageLeft = max(0, x);
    m_damageTop = max(0, y);
    m_damageRight = min(m_imageWidth, x+width);
    m_damageBottom = min(m_imageHeight, y+height);
  }
  m_isDamaged = true;
}

//--------------------------------------------------------------------
// damage entire bitmap
void mgXPSurface::damageAll()
{
  damage(0, 0, m_imageWidth, m_imageHeight);
}
  
//--------------------------------------------------------------------
// return bounds of damage
void mgXPSurface::getDamage(
  mgRectangle& bounds)
{
  bounds.m_x = m_damageLeft;
  bounds.m_y = m_damageTop;
  bounds.m_width = m_damageRight - m_damageLeft;
  bounds.m_height = m_damageBottom - m_damageTop;
}

//--------------------------------------------------------------------
// repair damage
void mgXPSurface::repair()
{
  if (m_isDamaged)
  {
    updateTiles(m_damageLeft, m_damageTop, m_damageRight, m_damageBottom);
    m_isDamaged = false;
  }
}

//--------------------------------------------------------------
// delete all the tiles
void mgXPSurface::deleteTiles()
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
// resize a tiled image
void mgXPSurface::setSurfaceSize(
  int width,
  int height)
{
  if (m_singleTile)
    resizeSingleImage(width, height);
  else resizeTiledImage(width, height);
  
  // clear the image to black
  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = m_imageWidth;
  rect.bottom = m_imageHeight;
  FillRect(m_surfaceDC, &rect, (HBRUSH) GetStockObject(BLACK_BRUSH));
  FillRect(m_alphaDC, &rect, (HBRUSH) GetStockObject(BLACK_BRUSH));

  // set the entire image area damaged
  damage(0, 0, m_imageWidth, m_imageHeight);
}

//--------------------------------------------------------------
// resize a tiled image
void mgXPSurface::resizeTiledImage(
  int width,
  int height)
{
  // figure number of tiles required
  int newHorzTiles = (width+SURFACE_TILE_SIZE-1)/SURFACE_TILE_SIZE;
  int newVertTiles = (height+SURFACE_TILE_SIZE-1)/SURFACE_TILE_SIZE;
  
  // if same number of tiles
  if (m_horzTiles * m_vertTiles == newHorzTiles * newVertTiles)
  {
    // adjust dimensions of image and tile array
    m_imageWidth = width;
    m_imageHeight = height;
    m_horzTiles = newHorzTiles;
    m_vertTiles = newVertTiles;
    
    return;
  }

  // delete the old bitmaps, different size
  if (m_surfaceBitmap != NULL)
  {
    SelectObject(m_surfaceDC, (HBITMAP) NULL);
    DeleteObject(m_surfaceBitmap);
    m_surfaceBitmap = NULL;

    SelectObject(m_alphaDC, (HBITMAP) NULL);
    DeleteObject(m_alphaBitmap);
    m_alphaBitmap = NULL;
  }

  // create bitmap info header 
  BITMAPINFO *pInfo = (BITMAPINFO *) new char[sizeof(BITMAPINFOHEADER)];

  pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pInfo->bmiHeader.biPlanes = 1;
  pInfo->bmiHeader.biBitCount = 32;
  pInfo->bmiHeader.biCompression = BI_RGB;
  pInfo->bmiHeader.biSizeImage = 0;
  pInfo->bmiHeader.biClrUsed = 0;
  pInfo->bmiHeader.biClrImportant = 0;

  // allocate image same size as tile grid.
  pInfo->bmiHeader.biWidth = newHorzTiles * SURFACE_TILE_SIZE;
  pInfo->bmiHeader.biHeight = -newVertTiles * SURFACE_TILE_SIZE;

  void *pData;
  m_surfaceBitmap = CreateDIBSection(m_surfaceDC, pInfo, DIB_PAL_COLORS, &pData, NULL, 0);
  m_surfaceData = (BYTE*) pData;
  SelectObject(m_surfaceDC, m_surfaceBitmap);

  m_alphaBitmap = CreateDIBSection(m_alphaDC, pInfo, DIB_PAL_COLORS, &pData, NULL, 0);
  m_alphaData = (BYTE*) pData;
  SelectObject(m_alphaDC, m_alphaBitmap);
  delete pInfo;

  int oldTileCount = m_horzTiles * m_vertTiles;

  // reallocate the tile array
  int tileCount = newHorzTiles*newVertTiles;
  mgTextureImage** newTiles = new mgTextureImage*[tileCount];
  
  // copy any old tiles we can reuse
  for (int i = 0; i < min(tileCount, oldTileCount); i++)
  {
    newTiles[i] = m_tiles[i];
  }
  
  // initialize additional new tiles
  for (int i = oldTileCount; i < tileCount; i++)
  {
    newTiles[i] = mgDisplay->createTextureMemory(SURFACE_TILE_SIZE, SURFACE_TILE_SIZE,
                      MG_MEMORY_FORMAT_BGRA, m_inworld);
  }
    
  // delete unused old tiles
  for (int i = tileCount; i < oldTileCount; i++)
  {
    delete m_tiles[i];
  }
  
  // switch to new tiles
  delete m_tiles;
  m_tiles = newTiles;
  m_horzTiles = newHorzTiles;
  m_vertTiles = newVertTiles;

  m_imageWidth = width;
  m_imageHeight = height;
}

//--------------------------------------------------------------
// set dimensions of image
void mgXPSurface::resizeSingleImage(
  int width,
  int height)
{
  m_imageWidth = width;
  m_imageHeight = height;

  // delete the old bitmap, different size
  if (m_surfaceBitmap != NULL)
  {
    SelectObject(m_surfaceDC, (HBITMAP) NULL);
    DeleteObject(m_surfaceBitmap);
    m_surfaceBitmap = NULL;

    SelectObject(m_alphaDC, (HBITMAP) NULL);
    DeleteObject(m_alphaBitmap);
    m_alphaBitmap = NULL;
  }

  // create bitmap info header 
  BITMAPINFO *pInfo = (BITMAPINFO *) new char[sizeof(BITMAPINFOHEADER)];

  pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pInfo->bmiHeader.biPlanes = 1;
  pInfo->bmiHeader.biBitCount = 32;
  pInfo->bmiHeader.biCompression = BI_RGB;
  pInfo->bmiHeader.biSizeImage = 0;
  pInfo->bmiHeader.biClrUsed = 0;
  pInfo->bmiHeader.biClrImportant = 0;

  pInfo->bmiHeader.biWidth = m_imageWidth;
  pInfo->bmiHeader.biHeight = -m_imageHeight;

  void *pData;
  m_surfaceBitmap = CreateDIBSection(m_surfaceDC, pInfo, DIB_PAL_COLORS, &pData, NULL, 0);
  m_surfaceData = (BYTE*) pData;
  SelectObject(m_surfaceDC, m_surfaceBitmap);

  m_alphaBitmap = CreateDIBSection(m_alphaDC, pInfo, DIB_PAL_COLORS, &pData, NULL, 0);
  m_alphaData = (BYTE*) pData;
  SelectObject(m_alphaDC, m_alphaBitmap);

  delete pInfo;

  deleteTiles();

  m_horzTiles = 1;
  m_vertTiles = 1;

  m_tiles = new mgTextureImage*[1];
  m_tiles[0] = mgDisplay->createTextureMemory(m_imageWidth, m_imageHeight,
                      MG_MEMORY_FORMAT_BGRA, m_inworld);
}

//--------------------------------------------------------------
// update texture tiles with new image
void mgXPSurface::updateTiles(
  int left,
  int top,
  int right,
  int bottom)
{
  GdiFlush();

  if (m_singleTile)
  {
    writeTile(m_tiles[0], 0, 0, m_imageWidth, m_imageHeight);
    return;
  }

  // break DIBitmap into texture tiles
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

      // if doesn't intersect the damage rectangle, no need to update
      if (min(right, hx) < max(left, lx) || 
          min(bottom, hy) < max(top, ly))
        continue;

      writeTile(m_tiles[i*m_horzTiles+j], lx, ly, wd, ht);
    }
  }
}

//--------------------------------------------------------------------
// write data of a tile
void mgXPSurface::writeTile(
  mgTextureImage* texture,
  int copyX,
  int copyY,
  int copyWidth,
  int copyHeight)
{
  if (texture == NULL)
    return;  

  // figure width of source bitmap
  int sourceStep;
  if (m_singleTile)
    sourceStep = m_imageWidth*4;
  else sourceStep = m_horzTiles*SURFACE_TILE_SIZE*4;

  BYTE* source = m_surfaceData + copyY*sourceStep + copyX*4;
  BYTE* sourceAlpha = m_alphaData + copyY*sourceStep + copyX*4;

  // to call updateTextureMemory, we need a contiguous block of memory
  int targetStep = copyWidth*4;
  BYTE* data = new BYTE[targetStep*copyHeight];
  BYTE* target = data; // +targetStep*(copyHeight-1);  // start of last scanline

  for (int i = 0; i < copyHeight; i++)
  {
    memcpy(target, source, targetStep);

    // copy the red channel from alpha plane into result
    for (int j = 0; j < copyWidth; j++)
    {
      target[4*j+3] = sourceAlpha[4*j+0];
    }

    target += targetStep;
    source += sourceStep;
    sourceAlpha += sourceStep;
  }

//  mgDisplay->m_textures->updateTextureMemory(texture, 0, texture->m_height - copyHeight, copyWidth, copyHeight, data);
  texture->updateMemory(0, 0, copyWidth, copyHeight, data);

  delete data;
}

//--------------------------------------------------------------------
// create buffers, ready to send to display
void mgXPSurface::createBuffers()
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
  updateTiles(0, 0, m_imageWidth, m_imageHeight);
}

//--------------------------------------------------------------------
// draw the surface as an overlay
void mgXPSurface::drawOverlay()
{
  if (m_singleTile)
  {
    mgDisplay->drawOverlayTexture(m_tiles[0], 0, 0, m_imageWidth, m_imageHeight);
    return;
  }

  // handle multi-tile case
  for (int i = 0; i < m_vertTiles; i++)
  {
    int y = SURFACE_TILE_SIZE * i;
    for (int j = 0; j < m_horzTiles; j++)
    {
      int x = SURFACE_TILE_SIZE * j;
      mgTextureImage* tile = m_tiles[i*m_horzTiles+j];
      mgDisplay->drawOverlayTexture(tile, x, y, tile->m_width, tile->m_height);
    }
  }
}

//--------------------------------------------------------------------
// delete any display buffers
void mgXPSurface::deleteBuffers()
{
  if (m_tiles == NULL)
    return;

  for (int i = 0; i < m_vertTiles * m_horzTiles; i++)
  {
    delete m_tiles[i];
    m_tiles[i] = NULL;
  }
}

#endif
