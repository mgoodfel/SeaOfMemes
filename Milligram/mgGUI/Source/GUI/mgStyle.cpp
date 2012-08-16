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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "GUI/mgStyle.h"

//-----------------------------------------------------------------
// constructor
mgStyle::mgStyle()
{
}

//-----------------------------------------------------------------
// destructor
mgStyle::~mgStyle()
{
}

//-----------------------------------------------------------------
// set a string attribute
void mgStyle::setAttr(
  const char* cntlName,
  const char* attrName,
  const char* value)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  m_stringAttrs.setAt(key, value);
}

//-----------------------------------------------------------------
// set an integer attribute
void mgStyle::setAttr(
  const char* cntlName,
  const char* attrName,
  int value)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  m_dwordAttrs.setAt(key, value);
}

//-----------------------------------------------------------------
// set a floating attribute
void mgStyle::setAttr(
  const char* cntlName,
  const char* attrName,
  double value)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  m_doubleAttrs.setAt(key, value);
}

//-----------------------------------------------------------------
// set a font attribute
void mgStyle::setAttr(
  const char* cntlName,
  const char* attrName,
  const mgFont* font)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  m_fontAttrs.setAt(key, font);
}

//-----------------------------------------------------------------
// set a color attribute
void mgStyle::setAttr(
  const char* cntlName,
  const char* attrName,
  const mgColor& color)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  m_colorAttrs.setAt(key, color.asRGB());
}

//-----------------------------------------------------------------
// set a pen attribute
void mgStyle::setAttr(
  const char* cntlName,
  const char* attrName,
  const mgPen* pen)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  m_penAttrs.setAt(key, pen);
}

//-----------------------------------------------------------------
// set a brush attribute
void mgStyle::setAttr(
  const char* cntlName,
  const char* attrName,
  const mgBrush* brush)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  m_brushAttrs.setAt(key, brush);
}

//-----------------------------------------------------------------
// set an icon attribute
void mgStyle::setAttr(
  const char* cntlName,
  const char* attrName,
  const mgIcon* icon)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  m_iconAttrs.setAt(key, icon);
}

//-----------------------------------------------------------------
// set a paint attribute
void mgStyle::setAttr(
  const char* cntlName,
  const char* attrName,
  const mgPaint* paint)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  m_paintAttrs.setAt(key, paint);
}

//-----------------------------------------------------------------
// set a frame attribute
void mgStyle::setAttr(
  const char* cntlName,
  const char* attrName,
  const mgFrame* frame)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  m_frameAttrs.setAt(key, frame);
}

//-----------------------------------------------------------------
// return a string attribute
BOOL mgStyle::getStringAttr(
  const char* cntlClass,
  const char* cntlName,
  const char* attrName,
  mgString& value)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  if (m_stringAttrs.lookup(key, value))
    return true;

  key.format("%s.%s", cntlClass, attrName);
  if (m_stringAttrs.lookup(key, value))
    return true;

  key.format("%s.%s", MG_STYLE_ALL, attrName);
  if (m_stringAttrs.lookup(key, value))
    return true;

  value.empty();
  return false;
}

//-----------------------------------------------------------------
// return an integer attribute
BOOL mgStyle::getIntegerAttr(
  const char* cntlClass,
  const char* cntlName,
  const char* attrName,
  int& value)
{
  DWORD v;
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  if (m_dwordAttrs.lookup(key, v))
  {
    value = (int) v;
    return true;
  }

  key.format("%s.%s", cntlClass, attrName);
  if (m_dwordAttrs.lookup(key, v))
  {
    value = (int) v;
    return true;
  }

  key.format("%s.%s", MG_STYLE_ALL, attrName);
  if (m_dwordAttrs.lookup(key, v))
  {
    value = (int) v;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------
// return a floating attribute
BOOL mgStyle::getDoubleAttr(
  const char* cntlClass,
  const char* cntlName,
  const char* attrName,
  double& value)
{
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  if (m_doubleAttrs.lookup(key, value))
    return true;

  key.format("%s.%s", cntlClass, attrName);
  if (m_doubleAttrs.lookup(key, value))
    return true;

  key.format("%s.%s", MG_STYLE_ALL, attrName);
  if (m_doubleAttrs.lookup(key, value))
    return true;

  value = 0.0;
  return false;
}

//-----------------------------------------------------------------
// return a font attribute
BOOL mgStyle::getFontAttr(
  const char* cntlClass,
  const char* cntlName,
  const char* attrName,
  const mgFont*& value)
{
  const void* v;
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  if (m_fontAttrs.lookup(key, v))
  {
    value = (const mgFont*) v;
    return true;
  }

  key.format("%s.%s", cntlClass, attrName);
  if (m_fontAttrs.lookup(key, v))
  {
    value = (const mgFont*) v;
    return true;
  }

  key.format("%s.%s", MG_STYLE_ALL, attrName);
  if (m_fontAttrs.lookup(key, v))
  {
    value = (const mgFont*) v;
    return true;
  }

  value = NULL;
  return false;
}

//-----------------------------------------------------------------
// return a color
BOOL mgStyle::getColorAttr(
  const char* cntlClass,
  const char* cntlName,
  const char* attrName,
  mgColor& value)
{
  DWORD v;
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  if (m_colorAttrs.lookup(key, v))
  {
    value = mgColor(v);
    return true;
  }

  key.format("%s.%s", cntlClass, attrName);
  if (m_colorAttrs.lookup(key, v))
  {
    value = mgColor(v);
    return true;
  }

  key.format("%s.%s", MG_STYLE_ALL, attrName);
  if (m_colorAttrs.lookup(key, v))
  {
    value = mgColor(v);
    return true;
  }

  value = mgColor("red");
  return false;
}

//-----------------------------------------------------------------
// return a brush attribute
BOOL mgStyle::getBrushAttr(
  const char* cntlClass,
  const char* cntlName,
  const char* attrName,
  const mgBrush*& value)
{
  const void* v;
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  if (m_brushAttrs.lookup(key, v))
  {
    value = (const mgBrush*) v;
    return true;
  }

  key.format("%s.%s", cntlClass, attrName);
  if (m_brushAttrs.lookup(key, v))
  {
    value = (const mgBrush*) v;
    return true;
  }

  key.format("%s.%s", MG_STYLE_ALL, attrName);
  if (m_brushAttrs.lookup(key, v))
  {
    value = (const mgBrush*) v;
    return true;
  }

  value = NULL;
  return false;
}

//-----------------------------------------------------------------
// return a pen attribute
BOOL mgStyle::getPenAttr(
  const char* cntlClass,
  const char* cntlName,
  const char* attrName,
  const mgPen*& value)
{
  const void* v;
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  if (m_penAttrs.lookup(key, v))
  {
    value = (const mgPen*) v;
    return true;
  }

  key.format("%s.%s", cntlClass, attrName);
  if (m_penAttrs.lookup(key, v))
  {
    value = (const mgPen*) v;
    return true;
  }

  key.format("%s.%s", MG_STYLE_ALL, attrName);
  if (m_penAttrs.lookup(key, v))
  {
    value = (const mgPen*) v;
    return true;
  }

  value = NULL;
  return false;
}

//-----------------------------------------------------------------
// return an icon attribute
BOOL mgStyle::getIconAttr(
  const char* cntlClass,
  const char* cntlName,
  const char* attrName,
  const mgIcon*& value)
{
  const void* v;
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  if (m_iconAttrs.lookup(key, v))
  {
    value = (const mgIcon*) v;
    return true;
  }

  key.format("%s.%s", cntlClass, attrName);
  if (m_iconAttrs.lookup(key, v))
  {
    value = (const mgIcon*) v;
    return true;
  }

  key.format("%s.%s", MG_STYLE_ALL, attrName);
  if (m_iconAttrs.lookup(key, v))
  {
    value = (const mgIcon*) v;
    return true;
  }

  value = NULL;
  return false;
}

//-----------------------------------------------------------------
// return a paint attribute
BOOL mgStyle::getPaintAttr(
  const char* cntlClass,
  const char* cntlName,
  const char* attrName,
  const mgPaint*& value)
{
  const void* v;
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  if (m_paintAttrs.lookup(key, v))
  {
    value = (const mgPaint*) v;
    return true;
  }

  key.format("%s.%s", cntlClass, attrName);
  if (m_paintAttrs.lookup(key, v))
  {
    value = (const mgPaint*) v;
    return true;
  }

  key.format("%s.%s", MG_STYLE_ALL, attrName);
  if (m_paintAttrs.lookup(key, v))
  {
    value = (const mgPaint*) v;
    return true;
  }

  value = NULL;
  return false;
}

//-----------------------------------------------------------------
// return a frame attribute
BOOL mgStyle::getFrameAttr(
  const char* cntlClass,
  const char* cntlName,
  const char* attrName,
  const mgFrame*& value)
{
  const void* v;
  mgString key;
  key.format("%s.%s", cntlName, attrName);
  if (m_frameAttrs.lookup(key, v))
  {
    value = (const mgFrame*) v;
    return true;
  }

  key.format("%s.%s", cntlClass, attrName);
  if (m_frameAttrs.lookup(key, v))
  {
    value = (const mgFrame*) v;
    return true;
  }

  key.format("%s.%s", MG_STYLE_ALL, attrName);
  if (m_frameAttrs.lookup(key, v))
  {
    value = (const mgFrame*) v;
    return true;
  }

  value = NULL;
  return false;
}
