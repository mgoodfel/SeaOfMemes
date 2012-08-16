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
#include <stdafx.h>

#ifdef SUPPORT_DX9
#include "mgDX9State.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgDX9Textures.h"
#include "Util/ImageUtil/ImageUtil.h"

//--------------------------------------------------------------
// constructor
mgDX9TextureImage::mgDX9TextureImage()
{
  m_handle = NULL;
}

//--------------------------------------------------------------
// destructor
mgDX9TextureImage::~mgDX9TextureImage()
{
  if (m_handle != NULL)
  {
    m_handle->Release();
    m_handle = NULL;
  }
}

//--------------------------------------------------------------
// constructor
mgDX9TextureArray::mgDX9TextureArray()
{
  m_textureCount = 0;
  m_atlasOrigins = NULL;
  m_atlasSizes = NULL;
  m_handle = NULL;
}

//--------------------------------------------------------------
// destructor
mgDX9TextureArray::~mgDX9TextureArray()
{
  if (m_handle != NULL)
  {
    m_handle->Release();
    m_handle = NULL;
  }

  delete m_atlasOrigins;
  m_atlasOrigins = NULL;

  delete m_atlasSizes;
  m_atlasSizes = NULL;
}

//--------------------------------------------------------------
// constructor
mgDX9Textures::mgDX9Textures()
{
}

//--------------------------------------------------------------
// destructor
mgDX9Textures::~mgDX9Textures()
{
  // free all the textures
  void* value;
  mgString key;
  int posn = m_nameToIDs.getStartPosition();
  while (posn != -1)
  {
    m_nameToIDs.getNextAssoc(posn, key, value);
    mgDX9TextureImage* texture = (mgDX9TextureImage*) value;
    delete texture;
  }

  m_nameToIDs.removeAll();

  // free all the texture arrays
  for (int i = 0; i < m_textureArrays.length(); i++)
  {
    mgDX9TextureArray* texture = (mgDX9TextureArray*) m_textureArrays[i];
    delete texture;
  }
  m_textureArrays.removeAll();
}

//--------------------------------------------------------------
// load texture from image file
mgTextureImage* mgDX9Textures::loadTexture(
  const char* fileName)
{
  void *value;
  // if we've already created the texture
  if (m_nameToIDs.lookup(fileName, value))
    return (mgTextureImage*) value;

  mgDX9TextureImage* texture = new mgDX9TextureImage();
  texture->m_fileName = fileName;
  mgOSFixFileName(texture->m_fileName);

  // if this is RGB file and alpha file
  int semi = texture->m_fileName.find(';');
  if (semi != -1)
  {
    mgString fileRGB(fileName, semi);
    mgString fileAlpha;
    texture->m_fileName.substring(fileAlpha, semi+1);

    if (!reloadRGBATextureFile(texture, fileRGB, fileAlpha))
    {
      delete texture;
      return NULL;
    }
  }
  else
  {
    // load RGB file
    if (!reloadTextureFile(texture, texture->m_fileName))
    {
      delete texture;
      return NULL;
    }
  }

  // save the texture id (under original name.  copy in texture is fixed)
  m_nameToIDs.setAt(fileName, texture);

  return texture;
}

//--------------------------------------------------------------
// load texture array from file list
mgTextureArray* mgDX9Textures::loadTextureArray(
  const mgPtrArray& fileList)
{
  mgDX9TextureArray* texture = new mgDX9TextureArray();

  // flag for each transparent texture
  texture->m_imgTransparent = new BOOL[fileList.length()];

  // position of textures within atlas
  texture->m_atlasOrigins = new mgPoint2[fileList.length()];
  texture->m_atlasSizes = new mgPoint2[fileList.length()];

  // copy the file list into the texture
  for (int i = 0; i < fileList.length(); i++)
  {
    mgString* fileName = (mgString*) fileList[i];
    mgString* copy = new mgString(*fileName);
    mgOSFixFileName(*copy);
    texture->m_fileList.add(copy);
    texture->m_imgTransparent[i] = false;
  }

  reloadTextureArray(texture);

  m_textureArrays.add(texture);
  return texture;
}

//--------------------------------------------------------------
// create texture to be updated from memory
mgTextureImage* mgDX9Textures::createTextureMemory(
  int width,
  int height,
  int format,
  BOOL mipmap)
{
  LPDIRECT3DTEXTURE9 handle = NULL;
  HRESULT hr = mg_d3dDevice->CreateTexture(
    width,
    height, 
    mipmap ? 0 : 1,   // levels
    mipmap ? D3DUSAGE_AUTOGENMIPMAP : 0,  // DWORD Usage,
    D3DFMT_A8R8G8B8, // D3DFORMAT Format,
    D3DPOOL_MANAGED, // D3DPOOL Pool,
    &handle, NULL); 

  if (FAILED(hr))
  {
    mgDebug("Failed to create texture.  rc=%d", hr);
    return NULL;
  }

  mgDX9TextureImage* texture = new mgDX9TextureImage();
  texture->m_width = width;
  texture->m_height = height;
  texture->m_mipmap = mipmap;
  texture->m_format = format;

  texture->m_handle = handle;

  return texture;
}

//--------------------------------------------------------------
// update memory texture
void mgDX9Textures::updateTextureMemory(
  mgTextureImage* texture,
  int x,
  int y,
  int width,
  int height,
  const BYTE* data)
{
  mgDX9TextureImage* ourTexture = (mgDX9TextureImage*) texture;

  // get access to handle data
  IDirect3DSurface9* targetSurface;
  HRESULT hr = ourTexture->m_handle->GetSurfaceLevel(0, &targetSurface);
  if (FAILED(hr))
    throw new mgException("Failed to get handle surface in updateTextureMemory. rc=%d", hr);

  D3DLOCKED_RECT lockedRect;
  hr = targetSurface->LockRect(
    &lockedRect, // D3DLOCKED_RECT *pLockedRect,
    NULL, // CONST RECT *pRect,
    D3DLOCK_DISCARD); // DWORD Flags

  if (FAILED(hr))
    throw new mgException("Failed to lock handle in updateTextureMemory. rc=%d", hr);

  // copy image data into handle
  BYTE* target = (BYTE*) lockedRect.pBits;
  target += lockedRect.Pitch * y + 4*x;

  const BYTE* source = data;
  for (int i = 0; i < height; i++)
  {
    if (ourTexture->m_format == mgDisplayTextures::MEMORY_FORMAT_BGRA)
    {
      memcpy(target, source, 4*width);
    }
    else
    {
      for (int j = 0; j < width; j++)
      {
        target[4*j+0] = source[4*j+2];  // blue
        target[4*j+1] = source[4*j+1];  // green
        target[4*j+2] = source[4*j+0];  // red
        target[4*j+3] = source[4*j+3];  // alpha
      }
    }
    target += lockedRect.Pitch;
    source += width*4;
  }
  
  hr = targetSurface->UnlockRect();
  if (FAILED(hr))
    throw new mgException("Failed to unlock handle in updateTextureMemory, rc=%d", hr);

  targetSurface->Release();
}

//--------------------------------------------------------------
// scale atlas of textures down by factor of 2.
void mgDX9Textures::scaleAtlas(
  int atlasWidth,
  int atlasHeight,
  BYTE* data)
{
  // just sum up 2 by 2 pixels.  Not the best scaling... :-)
  int sourceLineSize = atlasWidth*4;
  int targetLineSizse = (atlasWidth/2) * 4;
  BYTE* source = data;
  BYTE* target = data;
  for (int r = 0; r < atlasHeight; r+=2)
  {
    BYTE* nextLine = source + sourceLineSize;
    for (int c = 0; c < atlasWidth; c+= 2)
    {
      int avgR = (source[0] + source[4] + nextLine[0] + nextLine[4])/4;
      int avgG = (source[1] + source[5] + nextLine[1] + nextLine[5])/4;
      int avgB = (source[2] + source[6] + nextLine[2] + nextLine[6])/4;
      int avgA = (source[3] + source[7] + nextLine[3] + nextLine[7])/4;

      // advance source by two pixels
      source += 2*4;
      nextLine += 2*4;

      // write target
      target[0] = avgR;
      target[1] = avgG;
      target[2] = avgB;
      target[3] = avgA;
      target += 4;  // one pixel
    }
    // we've advanced one line in loop, skip second line
    source += sourceLineSize;  
  }
}

const int ATLAS_LEVELS = 7;
const int ATLAS_MARGIN = 1 << ATLAS_LEVELS;

//--------------------------------------------------------------
// reload texture array from file list
void mgDX9Textures::reloadTextureArray(
  mgDX9TextureArray* texture)
{
  if (texture->m_fileList.length() == 0)
    return;

  // get the first texture file, for size
  mgString fileName = texture->m_fileList[0];
  int imgWidth, imgHeight;
  BYTE* data;
  if (!mgLoadRGBAImage(*fileName, imgWidth, imgHeight, data))
    throw new mgException("could not load array texture %s", (const char*) fileName);
    
  int cellWidth = imgWidth;
  int cellHeight = imgHeight;

  int fullWidth = 2*ATLAS_MARGIN + imgWidth;
  int fullHeight = 2*ATLAS_MARGIN + imgHeight;

  // make the texture square-ish
  int colCount = (int) (0.5+sqrt((double) texture->m_fileList.length()));
  colCount = min(colCount, 2048/fullWidth);   // =-= query max texture width
  int atlasWidth = colCount*fullWidth;

  int rowCount = (texture->m_fileList.length()+colCount-1) / colCount;
  int atlasHeight = rowCount*fullHeight;

  // allocate data for the complete texture atlas
  int atlasLineSize = 4*atlasWidth;
  BYTE* atlas = new BYTE[atlasLineSize * atlasHeight];
  memset(atlas, 0, atlasLineSize * atlasHeight);

  // load the images in the file list into the array
  double tw = (double) atlasWidth;
  double th = (double) atlasHeight;

  int col = ATLAS_MARGIN;
  int row = ATLAS_MARGIN;
  for (int i = 0; i < texture->m_fileList.length(); i++)
  {
    mgString fileName = texture->m_fileList[i];

    // =-= tacky, but this indicates transparent image, since it has an alpha image file
    texture->m_imgTransparent[i] = fileName->find(';') != -1;

    // first texture's data already loaded, so skip i == 0
    if (i != 0 && !mgLoadRGBAImage(*fileName, imgWidth, imgHeight, data))
      throw new mgException("reloadTextureArray could not load texture %s", (const char*) fileName);

    if (imgWidth != cellWidth || imgHeight != cellHeight)
      throw new mgException("reloadTextureArray file %s has size %d by %d, expected %d by %d", 
        (const char*) *fileName, imgWidth, imgHeight, cellWidth, cellHeight);

    // set origin and size of texture in atlas
    texture->m_atlasOrigins[i].x = col/tw;
    texture->m_atlasOrigins[i].y = row/th;
    texture->m_atlasSizes[i].x = imgWidth/tw;
    texture->m_atlasSizes[i].y = imgHeight/th;

    // copy the data into the altas
    BYTE* source = data;
    BYTE* target = atlas + atlasLineSize * row + 4*col;
    int imgLineSize = imgWidth * 4;

    int horzMarginSize = 4*ATLAS_MARGIN;
    int tgtOffset = -horzMarginSize;
    for (int r = 0; r < imgHeight; r++)
    {
      // copy right side of image to left margin
      memcpy(target + tgtOffset, source + imgLineSize-horzMarginSize, horzMarginSize);
      tgtOffset += horzMarginSize;

      // copy image
      memcpy(target + tgtOffset, source, imgLineSize);
      tgtOffset += imgLineSize;

      // copy left side of image to right margin
      memcpy(target + tgtOffset, source, horzMarginSize);
      tgtOffset += horzMarginSize;

      // advance target to start of next scanline
      tgtOffset += atlasLineSize - 4*fullWidth;

      // source to next scanline
      source += imgLineSize;
    }

    // we started at (row, col), which is the image itself.  move to left edge of cell
    target -= horzMarginSize;

    int fullLineSize = 4*fullWidth;
    for (int r = 0; r < ATLAS_MARGIN; r++)
    {
      // copy margin scanline from bottom to top
      int bottom = (imgHeight - ATLAS_MARGIN) + r;
      int top = r-ATLAS_MARGIN;   // target is first image scanline, not margin
      memcpy(target+top*atlasLineSize, target+bottom*atlasLineSize, fullLineSize);

      // copy margin scanlines from top to bottom
      top = r;
      bottom = imgHeight+r;
      memcpy(target+bottom*atlasLineSize, target+top*atlasLineSize, fullLineSize);
    }
    delete data;

    col += fullWidth;
    if (col >= atlasWidth)
    {
      col = ATLAS_MARGIN;
      row += fullHeight;
    }
  }

  // create the texture
  HRESULT hr = mg_d3dDevice->CreateTexture(
    atlasWidth, atlasHeight, 
    ATLAS_LEVELS, 
    0, // D3DUSAGE_AUTOGENMIPMAP,  // DWORD Usage,
    D3DFMT_A8R8G8B8, // D3DFORMAT Format,
    D3DPOOL_MANAGED, // D3DPOOL Pool,
    &texture->m_handle, NULL); 

  if (FAILED(hr))
    throw new mgException("reloadTextureArray failed to create texture. rc=%d", hr);

  for (int level = 0; level < ATLAS_LEVELS; level++)
  {
    // get access to handle data
    D3DLOCKED_RECT lockedRect;
    hr = texture->m_handle->LockRect(level, &lockedRect, NULL, D3DLOCK_DISCARD);
    if (FAILED(hr))
      throw new mgException("reloadTextureArray failed to LockRect. rc=%d", hr);
    
    // copy the texture into the level.  DirectX wants BGRA format, and we have RGBA
    BYTE* target = (BYTE*) lockedRect.pBits;
    BYTE* source = atlas;
    for (int r = 0; r < atlasHeight; r++)
    {
      for (int c = 0; c < atlasWidth; c++)
      {
        target[0] = source[2];  // blue
        target[1] = source[1];  // green
        target[2] = source[0];  // red
        target[3] = source[3];  // alpha
        target += 4;
        source += 4;
      }
      target += lockedRect.Pitch - atlasLineSize;
    }
    hr = texture->m_handle->UnlockRect(level);
    if (FAILED(hr))
      throw new mgException("reloadTextureArray failed to UnlockRect. rc=%d", hr);

    scaleAtlas(atlasWidth, atlasHeight, atlas);
    atlasWidth /= 2;
    atlasHeight /= 2;
    atlasLineSize = 4*atlasWidth;
  }

  delete atlas;
}

//--------------------------------------------------------------
// reload texture image file
BOOL mgDX9Textures::reloadTextureFile(
  mgDX9TextureImage* texture,
  const char* fileName)
{
  BYTE* data;
  int width;
  int height;
  if (!mgLoadImage(fileName, width, height, data))
  {
    mgDebug("could not load texture image file %s", (const char*) fileName);
    return false;
  }

  texture->m_width = width;
  texture->m_height = height;
  texture->m_handle = createTextureRGB(width, height, data);
  delete data;
  
  return true;
}

//--------------------------------------------------------------
// load RGBA texture in two files
BOOL mgDX9Textures::reloadRGBATextureFile(
  mgDX9TextureImage* texture,
  const char* fileRGB,
  const char* fileAlpha)
{
  BYTE* dataRGB = NULL;
  int width, height;
  if (!mgLoadImage(fileRGB, width, height, dataRGB))
  {
    mgDebug("could not load %s", (const char*) fileRGB);
    return false;
  }

  BYTE* dataAlpha = NULL;
  int alphaWidth, alphaHeight;
  if (!mgLoadImage(fileAlpha, alphaWidth, alphaHeight, dataAlpha))
  {
    delete dataRGB;
    mgDebug("could not load %s", (const char*) fileAlpha);
    return false;
  }

  if (alphaWidth != width || alphaHeight != height)
  {
    delete dataRGB;
    delete dataAlpha;
    mgDebug("image %s and alpha image %s sizes different.", (const char*) fileRGB, (const char*) fileAlpha);
  }

  BYTE* data = new BYTE[width*height*4];
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      int source = (i*width+j)*3;
      int target = (i*width+j)*4;
      data[target+0] = dataRGB[source+0];
      data[target+1] = dataRGB[source+1];
      data[target+2] = dataRGB[source+2];
      data[target+3] = dataAlpha[source];
    }
  }

  texture->m_width = width;
  texture->m_height = height;
  texture->m_handle = createTextureRGBA(width, height, data);
  texture->m_transparent = true;

  delete dataRGB;
  delete dataAlpha;
  delete data;

  return true;
}

//--------------------------------------------------------------
// create a texture and load with data
LPDIRECT3DTEXTURE9 mgDX9Textures::createTextureRGB(
  int width,
  int height,
  BYTE* data)
{
  // create the texture
  LPDIRECT3DTEXTURE9 handle = NULL;
  HRESULT hr = mg_d3dDevice->CreateTexture(
    width,
    height, 
    0,   // levels
    D3DUSAGE_AUTOGENMIPMAP,  // DWORD Usage,
    D3DFMT_X8R8G8B8, // D3DFORMAT Format,
    D3DPOOL_MANAGED, // D3DPOOL Pool,
    &handle, NULL); 

  if (FAILED(hr))
  {
    mgDebug("Failed to create texture. rc=%d", hr);
    return NULL;
  }

  // get access to handle data
  IDirect3DSurface9* targetSurface;
  hr = handle->GetSurfaceLevel(0, &targetSurface);
  if (FAILED(hr))
  {
    mgDebug("Failed to get handle surface. rc=%d", hr);
    return NULL;
  }

  D3DLOCKED_RECT lockedRect;
  hr = targetSurface->LockRect(
    &lockedRect, // D3DLOCKED_RECT *pLockedRect,
    NULL, // CONST RECT *pRect,
    D3DLOCK_DISCARD); // DWORD Flags

  if (FAILED(hr))
  {
    mgDebug("Failed to lock handle. rc=%d", hr);
    return NULL;
  }

  // copy image data into handle
  BYTE* target = (BYTE*) lockedRect.pBits;
  BYTE* source = data;
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      target[4*j+0] = source[3*j+2];
      target[4*j+1] = source[3*j+1];
      target[4*j+2] = source[3*j+0];
      target[4*j+3] = 0;
    }
    target += lockedRect.Pitch;
    source += width*3;
  }

  hr = targetSurface->UnlockRect();
  if (FAILED(hr))
  {
    mgDebug("Failed to unlock handle. rc=%d", hr);
    return NULL;
  }
  targetSurface->Release();

  return handle;
}

//--------------------------------------------------------------
// create a texture and load with data
LPDIRECT3DTEXTURE9 mgDX9Textures::createTextureRGBA(
  int width,
  int height,
  BYTE* data)
{
  // create the handle
  LPDIRECT3DTEXTURE9 handle = NULL;
  HRESULT hr = mg_d3dDevice->CreateTexture(
    width,
    height, 
    0,   // levels
    D3DUSAGE_AUTOGENMIPMAP,  // DWORD Usage,
    D3DFMT_A8R8G8B8, // D3DFORMAT Format,
    D3DPOOL_MANAGED, // D3DPOOL Pool,
    &handle, NULL); 

  if (FAILED(hr))
  {
    mgDebug("Failed to create texture.  rc=%d", hr);
    return NULL;
  }

  // get access to handle data
  IDirect3DSurface9* targetSurface;
  hr = handle->GetSurfaceLevel(0, &targetSurface);
  if (FAILED(hr))
  {
    mgDebug("Failed to get handle surface. rc=%d", hr);
    return NULL;
  }

  D3DLOCKED_RECT lockedRect;
  hr = targetSurface->LockRect(
    &lockedRect, // D3DLOCKED_RECT *pLockedRect,
    NULL, // CONST RECT *pRect,
    D3DLOCK_DISCARD); // DWORD Flags

  if (FAILED(hr))
  {
    mgDebug("Failed to lock handle. rc=%d", hr);
    return NULL;
  }

  // copy image data into handle
  BYTE* target = (BYTE*) lockedRect.pBits;
  BYTE* source = data;
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      target[4*j+0] = source[4*j+2];  // blue
      target[4*j+1] = source[4*j+1];  // green
      target[4*j+2] = source[4*j+0];  // red
      target[4*j+3] = source[4*j+3];  // alpha
    }
    target += lockedRect.Pitch;
    source += width*4;
  }
  
  hr = targetSurface->UnlockRect();
  if (FAILED(hr))
  {
    mgDebug("Failed to unlock handle. rc=%d", hr);
    return NULL;
  }
  targetSurface->Release();

  return handle;
}

//--------------------------------------------------------------
// release device resources
void mgDX9Textures::deleteBuffers()
{
  // release texture data
  void* value;
  mgString key;
  int posn = m_nameToIDs.getStartPosition();
  while (posn != -1)
  {
    m_nameToIDs.getNextAssoc(posn, key, value);
    mgDX9TextureImage* texture = (mgDX9TextureImage*) value;
    if (texture->m_handle != NULL)
    {
      texture->m_handle->Release();
      texture->m_handle = NULL;
    }
  }

  for (int i = 0; i < m_textureArrays.length(); i++)
  {
    mgDX9TextureArray* texture = (mgDX9TextureArray*) m_textureArrays[i];
    if (texture->m_handle != NULL)
    {
      texture->m_handle->Release();
      texture->m_handle = NULL;
    }
  }
}

//--------------------------------------------------------------
// restore display resources
void mgDX9Textures::createBuffers()
{
  // restore textures from source file
  void* value;
  mgString key;
  int posn = m_nameToIDs.getStartPosition();
  while (posn != -1)
  {
    m_nameToIDs.getNextAssoc(posn, key, value);
    mgDX9TextureImage* texture = (mgDX9TextureImage*) value;
    
    // parse the key into one or two file names
    int semi = key.find(';');
    if (semi != -1)
    {
      // reload pair of files as texture
      mgString rgbFile, alphaFile;
      key.substring(rgbFile, 0, semi);
      key.substring(alphaFile, semi+1);

      reloadRGBATextureFile(texture, rgbFile, alphaFile);
    }
    else
    { 
      // reload single file as texture
      reloadTextureFile(texture, key);
    }
  }

  // reload texture arrays
  for (int i = 0; i < m_textureArrays.length(); i++)
  {
    mgDX9TextureArray* texture = (mgDX9TextureArray*) m_textureArrays[i];
    reloadTextureArray(texture);
  }
}

#endif
