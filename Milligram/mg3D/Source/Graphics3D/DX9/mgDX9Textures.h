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
#if !defined(MGDX9TEXTURES_H) && defined(SUPPORT_DX9)
#define MGDX9TEXTURES_H

#include "mgDX9Display.h"
#include "Graphics3D/mgDisplayTextures.h"

class mgDX9TextureImage : public mgTextureImage
{
public:
  LPDIRECT3DTEXTURE9 m_handle;
  int m_format;
  BOOL m_mipmap;

  // constructor
  mgDX9TextureImage();

  // destructor
  virtual ~mgDX9TextureImage();
};

class mgDX9TextureArray : public mgTextureArray
{
public:
  LPDIRECT3DTEXTURE9 m_handle;
  int m_textureCount;
  mgPoint2* m_atlasOrigins;
  mgPoint2* m_atlasSizes;

  // constructor
  mgDX9TextureArray();

  // destructor
  virtual ~mgDX9TextureArray();
};

class mgDX9Textures : public mgDisplayTextures
{
public:
  // constructor and destructor
  mgDX9Textures();
  virtual ~mgDX9Textures();

  // load/create texture from file
  virtual mgTextureImage* loadTexture(
    const char* fileName);

  // load texture array from file list
  virtual mgTextureArray* loadTextureArray(
    const mgPtrArray& fileList);          // array of mgString*

  // create texture to be updated from memory
  virtual mgTextureImage* createTextureMemory(
    int width,
    int height,
    int format,
    BOOL mipmap);

  // update memory texture
  virtual void updateTextureMemory(
    mgTextureImage* texture,
    int x,
    int y,
    int width,
    int height,
    const BYTE* data);

  // delete any display buffers
  virtual void deleteBuffers();
  
  // create buffers, ready to send to display
  virtual void createBuffers();

protected:
  mgMapStringToPtr m_nameToIDs; // map file name to texture id
  mgPtrArray m_textureArrays;   // list of texture arrays

  // reload texture image file
  BOOL reloadTextureFile(
    mgDX9TextureImage* texture,
    const char* fileName);
    
  // reload RGBA texture from two files
  BOOL reloadRGBATextureFile(
    mgDX9TextureImage* texture,
    const char* fileRGB,
    const char* fileAlpha);

  // create a texture and load with data
  LPDIRECT3DTEXTURE9 createTextureRGB(
    int width,
    int height,
    BYTE* data);

  // create a texture and load with data
  LPDIRECT3DTEXTURE9 createTextureRGBA(
    int width,
    int height,
    BYTE* data);

  // scale atlas of textures down by factor of 2.
  void scaleAtlas(
    int atlasWidth,
    int atlasHeight,
    BYTE* data);

  // reload the texture array
  void reloadTextureArray(
    mgDX9TextureArray* texture);
};

#endif
