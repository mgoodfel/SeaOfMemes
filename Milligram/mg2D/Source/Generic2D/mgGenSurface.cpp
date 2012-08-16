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

#include "ft2build.h"
#include FT_FREETYPE_H

#include "mgGenContext.h"
#include "mgGenImage.h"
#include "mgGenIcon.h"
#include "mgGenCharCache.h"
#include "Graphics2D/Surfaces/mgGenSurface.h"

//--------------------------------------------------------------
// public constructor only creates inworld-single tile images
mgGenSurface::mgGenSurface()
{
  m_imageData = NULL;
  m_isDamaged = false;

  m_imageWidth = 0;
  m_imageHeight = 0;

  // initialize instance of free type library
  int error = FT_Init_FreeType((FT_Library*) &m_ftLibrary);
  if (error != 0)
    throw new mgException("FT_Init_FreeType returns %d", error);

  m_charCache = new mgGenCharCache();
}

//--------------------------------------------------------------
// destructor
mgGenSurface::~mgGenSurface()
{
  // delete the surface data
  delete m_imageData;
  m_imageData = NULL;

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

  FT_Done_FreeType((FT_Library) m_ftLibrary);

  delete m_charCache;
  m_charCache = NULL;
}

//--------------------------------------------------------------
// get surface pixels, 32-bits per pixel (DWORD=RGBA)
void mgGenSurface::getPixels(
  int& imageWidth,
  int& imageHeight,
  DWORD*& imageData) const
{
  imageWidth = m_imageWidth;
  imageHeight = m_imageHeight;
  imageData = m_imageData;
}

//--------------------------------------------------------------
// convert points to pixels on this device
int mgGenSurface::points(
  double ptSize) const
{
  return (int) floor(0.5+(ptSize*displayDPI())/72.0);
}      

//--------------------------------------------------------------
// create font
const mgFont* mgGenSurface::createFont(
  const char* faceName,
  int size,
  BOOL bold,
  BOOL italic)
{
  // construct name used for hashtable keys
  mgString key;
  key.format("font-%s-%d%s%s", (const char*) faceName, size, bold?"-B":"", italic?"-I":"");

  // if found, we're done
  mgResource* value = findResource(key);
  if (value != NULL)
    return (mgFont*) value;

  // create font and add to cache
  mgGenFont* font = new mgGenFont(this, faceName, size, bold, italic);
  font->m_key = key;

  saveResource(font);

  return font;
}

//--------------------------------------------------------------
// create a font from "arial-10-b-i" style fontspec
const mgFont* mgGenSurface::createFont(
  const char* spec)
{
  // parse fontSpec
  mgString faceName;
  int size = 12;
  BOOL bold = false;
  BOOL italic = false;
  
  // parse fontSpec as facename-size-B-I
  mgString fontSpec(spec);
  int posn = fontSpec.find(0, "-");
  if (posn < fontSpec.length())
  {
    fontSpec.substring(faceName, 0, posn);
    posn = fontSpec.nextLetter(posn);
  }
  else faceName = fontSpec;

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
      throw new mgErrorMsg("2dBadFontSpec", "spec", "%s", (const char*) fontSpec);
  }

  char letter[MG_MAX_LETTER];
  while (posn < fontSpec.length())
  {
    posn = fontSpec.nextLetter(posn, letter);
    if (strcmp(letter, "-") != 0)
      throw new mgErrorMsg("2dBadFontSpec", "spec", "%s", (const char*) fontSpec);

    posn = fontSpec.nextLetter(posn, letter);
    if (_stricmp(letter, "B") == 0)
      bold = true;
    else if (_stricmp(letter, "I") == 0)
      italic = true;
    else throw new mgErrorMsg("2dBadFontSpec", "spec", "%s", (const char*) fontSpec);
  }
  
  return createFont(faceName, size, bold, italic);
}

//--------------------------------------------------------------
// create a brush
const mgBrush* mgGenSurface::createBrush(
  int r, 
  int g, 
  int b)
{
  mgString key;
  key.format("brush-%d,%d,%d", r, g, b);
  
  mgResource* value = findResource(key);
  if (value != NULL)
    return (mgBrush*) value;

  mgGenBrush* brush = new mgGenBrush(r, g, b);
  brush->m_key = key;
  saveResource(brush);

  return brush;
}
    
//--------------------------------------------------------------
// create a brush
const mgBrush* mgGenSurface::createBrush(
  const mgColor& color)
{
  return createBrush(color.m_r, color.m_g, color.m_b);
}
    
//--------------------------------------------------------------
// create a brush
const mgBrush* mgGenSurface::createBrush(
  const char* colorSpec)
{
  mgColor color(colorSpec);
  return createBrush(color.m_r, color.m_g, color.m_b);
}

//--------------------------------------------------------------
// create pen from color
const mgPen* mgGenSurface::createPen(
  int r, 
  int g, 
  int b,
  int thick)
{
  mgString key;
  key.format("pen-%d,%d,%d,%d", r, g, b, thick);
  
  mgResource* value = findResource(key);
  if (value != NULL)
    return (mgPen*) value;

  // create the pen
  mgGenPen* pen = new mgGenPen(r, g, b, thick);
  pen->m_key = key;
  saveResource(pen);
  
  return pen;
}
    
//--------------------------------------------------------------
// create named color pen
const mgPen* mgGenSurface::createPen(
  const char* colorSpec,
  int thick)
{
  mgColor color(colorSpec);
  return createPen(color.m_r, color.m_g, color.m_b, thick);
}

//--------------------------------------------------------------
// create pen from color
const mgPen* mgGenSurface::createPen(
  const mgColor& color,
  int thick)
{
  return createPen(color.m_r, color.m_g, color.m_b, thick);
}

//--------------------------------------------------------------
// create an icon
const mgIcon* mgGenSurface::createIcon(
  const char* fileName)
{
  mgString key;
  key.format("icon-%s", (const char*) fileName);
  mgResource* value = findResource(key);
  if (value != NULL)
    return (mgIcon*) value;

  // create the icon
  mgGenIcon* icon = new mgGenIcon(fileName);
  icon->m_key = key;
  saveResource(icon);
  
  return icon;
}

//--------------------------------------------------------------
// create an image
const mgImage* mgGenSurface::createImage(
  const char* fileName)
{
  return new mgGenImage(fileName);
}

//--------------------------------------------------------------
// save a resource
void mgGenSurface::saveResource(
  const mgResource* data)
{
  m_resourceCache.setAt(data->m_key, data);
}

//--------------------------------------------------------------
// find a resource
mgResource* mgGenSurface::findResource(
  const char* name) const
{
  const void* value;
  if (m_resourceCache.lookup(name, value))
    return (mgResource*) value;

  return NULL;
}

//--------------------------------------------------------------
// remove a resource
void mgGenSurface::removeResource(
  const mgResource* data)
{
  m_resourceCache.removeKey(data->m_key);
}

//--------------------------------------------------------------
// create a drawing context
mgContext* mgGenSurface::newContext()
{
  return (mgContext*) new mgGenContext(this);
}
  
//--------------------------------------------------------------------
// damage a rectangle 
void mgGenSurface::damage(
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
void mgGenSurface::damageAll()
{
  damage(0, 0, m_imageWidth, m_imageHeight);
}
  
//--------------------------------------------------------------------
// return bounds of damage
void mgGenSurface::getDamage(
  mgRectangle& bounds) const
{
  bounds.m_x = m_damageLeft;
  bounds.m_y = m_damageTop;
  bounds.m_width = m_damageRight - m_damageLeft;
  bounds.m_height = m_damageBottom - m_damageTop;
}

//--------------------------------------------------------------------
// return true if damaged
BOOL mgGenSurface::isDamaged() const
{
  return m_isDamaged;
} 

//--------------------------------------------------------------------
// repair damage
void mgGenSurface::repair(
  mgRectangle& bounds)
{
  if (m_isDamaged)
  {
    bounds.m_x = m_damageLeft;
    bounds.m_y = m_damageTop;
    bounds.m_width = m_damageRight - m_damageLeft;
    bounds.m_height = m_damageBottom - m_damageTop;

    m_isDamaged = false;
  }
}

//--------------------------------------------------------------
// resize the surface
void mgGenSurface::setSurfaceSize(
  int width,
  int height)
{
  // reallocate the surface data
  if (width != m_imageWidth || height != m_imageHeight)
  {
    delete m_imageData;
    m_imageData = NULL;

    m_imageWidth = width;
    m_imageHeight = height;
    m_imageData = new DWORD[m_imageWidth * m_imageHeight];

    // clear the image to black
    memset(m_imageData, 0, m_imageWidth*m_imageHeight*sizeof(DWORD));
  }

  // set the entire image area damaged
  damage(0, 0, m_imageWidth, m_imageHeight);
}

//--------------------------------------------------------------
// return surface size
void mgGenSurface::getSurfaceSize(
  int& width,
  int& height) const
{
  width = m_imageWidth;
  height = m_imageHeight;
}
