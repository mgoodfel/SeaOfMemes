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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Surfaces/mgGenContext.h"
#include "Surfaces/mgGenFont.h"
#include "Surfaces/mgGenImage.h"
#include "Surfaces/mgGenIcon.h"
#include "Surfaces/mgGenSurface.h"

//--------------------------------------------------------------
// public constructor only creates inworld-single tile images
mgGenSurface::mgGenSurface()
{
  m_isDamaged = false;

  m_surfaceWidth = 0;
  m_surfaceHeight = 0;

}

//--------------------------------------------------------------
// destructor
mgGenSurface::~mgGenSurface()
{
  removeAllResources();
}

//--------------------------------------------------------------
// free all the resources
void mgGenSurface::removeAllResources()
{
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
  const mgColor& color)
{
  mgString key;
  key.format("brush-%.4f,%.4f,%.4f,%.4f", color.m_r, color.m_g, color.m_b, color.m_a);
  
  mgResource* value = findResource(key);
  if (value != NULL)
    return (mgBrush*) value;

  mgBrush* brush = new mgBrush(color);
  brush->m_key = key;
  saveResource(brush);

  return brush;
}
    
//--------------------------------------------------------------
// create a brush
const mgBrush* mgGenSurface::createBrush(
  double r, 
  double g, 
  double b,
  double a)
{
  return createBrush(mgColor(r, g, b, a));
}
    
//--------------------------------------------------------------
// create a brush
const mgBrush* mgGenSurface::createBrush(
  const char* colorSpec)
{
  return createBrush(mgColor(colorSpec));
}

//--------------------------------------------------------------
// create pen from color
const mgPen* mgGenSurface::createPen(
  double thick,
  const mgColor& color)
{
  mgString key;
  key.format("pen-%.4f,%.4f,%.4f,%.4f,%.4f", thick, color.m_r, color.m_g, color.m_b, color.m_a);
  
  mgResource* value = findResource(key);
  if (value != NULL)
    return (mgPen*) value;

  // create the pen
  mgPen* pen = new mgPen(thick, color);
  pen->m_key = key;
  saveResource(pen);
  
  return pen;
}
    
//--------------------------------------------------------------
// create named color pen
const mgPen* mgGenSurface::createPen(
  double thick,
  const char* colorSpec)
{
  return createPen(thick, mgColor(colorSpec));
}

//--------------------------------------------------------------
// create pen from color
const mgPen* mgGenSurface::createPen(
  double thick,
  double r, 
  double g, 
  double b,
  double a)
{
  return createPen(thick, mgColor(r, g, b, a));
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
  mgGenIcon* icon = new mgGenIcon(this);
  icon->m_fileName = fileName;
  icon->m_handle = loadIcon(icon);
  icon->m_key = key;
  saveResource(icon);
  
  return icon;
}

//--------------------------------------------------------------
// create an image
const mgImage* mgGenSurface::createImage(
  const char* fileName)
{
  mgGenImage* image = new mgGenImage(this);
  image->m_fileName = fileName;
  image->m_handle = loadImage(image);
  return image;
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
    m_damageRight = min(m_surfaceWidth, max(m_damageRight, x+width));
    m_damageBottom = min(m_surfaceHeight, max(m_damageBottom, y+height));
  }
  else
  {
    m_damageLeft = max(0, x);
    m_damageTop = max(0, y);
    m_damageRight = min(m_surfaceWidth, x+width);
    m_damageBottom = min(m_surfaceHeight, y+height);
  }
  m_isDamaged = true;
}

//--------------------------------------------------------------------
// damage entire bitmap
void mgGenSurface::damageAll()
{
  damage(0, 0, m_surfaceWidth, m_surfaceHeight);
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
  if (width != m_surfaceWidth || height != m_surfaceHeight)
  {
    m_surfaceWidth = width;
    m_surfaceHeight = height;
  }

  // set the entire image area damaged
  damage(0, 0, m_surfaceWidth, m_surfaceHeight);
}

//--------------------------------------------------------------
// return surface size
void mgGenSurface::getSurfaceSize(
  int& width,
  int& height) const
{
  width = m_surfaceWidth;
  height = m_surfaceHeight;
}

