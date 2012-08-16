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

#include "Graphics2D/mgColor.h"

//--------------------------------------------------------------
// constructor
mgColor::mgColor(
  const char* spec)
{
  // unknown color is red
  m_r = 255; m_g = 0; m_b = 0;

  if (_stricmp(spec, "black") == 0)
  {  m_r = 0; m_g = 0; m_b = 0; }

  else if (_stricmp(spec, "white") == 0)
  {  m_r = 255; m_g = 255; m_b = 255; }

  else if (_stricmp(spec, "lightGray") == 0)
  {  m_r = 192; m_g = 192; m_b = 192; }

  else if (_stricmp(spec, "lightGrey") == 0)
  {  m_r = 192; m_g = 192; m_b = 192; }

  else if (_stricmp(spec, "gray") == 0)
  {  m_r = 192; m_g = 192; m_b = 192; }

  else if (_stricmp(spec, "grey") == 0)
  {  m_r = 192; m_g = 192; m_b = 192; }

  else if (_stricmp(spec, "darkGray") == 0)
  {  m_r = 128; m_g = 128; m_b = 128; }

  else if (_stricmp(spec, "darkGrey") == 0)
  {  m_r = 128; m_g = 128; m_b = 128; }

  else if (_stricmp(spec, "red") == 0)
  {  m_r = 255; m_g = 0; m_b = 0; }

  else if (_stricmp(spec, "green") == 0)
  {  m_r = 0; m_g = 255; m_b = 0; }

  else if (_stricmp(spec, "blue") == 0)
  {  m_r = 0; m_g = 0; m_b = 255; }

  else if (_stricmp(spec, "yellow") == 0)
  {  m_r = 255; m_g = 255; m_b = 0; }

  else if (_stricmp(spec, "cyan") == 0)
  {  m_r = 255; m_g = 0; m_b = 255; }

  else if (_stricmp(spec, "magenta") == 0)
  {  m_r = 0; m_g = 255; m_b = 255; }

  else if (spec[0] == '#')
  {
    int value;
    if (1 == sscanf(spec+1, "%x", &value))
    {
      m_r = (value >> 16) & 0xFF;
      m_g = (value >> 8) & 0xFF;
      m_b = value & 0xFF;
    }
  }
}

//--------------------------------------------------------------
// constructor
mgColor::mgColor()
{
  m_r = m_g = m_b = 0;
}
  
//--------------------------------------------------------------
// constructor
mgColor::mgColor(
  BYTE r,
  BYTE g,
  BYTE b)
{
  m_r = r;
  m_g = g;
  m_b = b;
}

//--------------------------------------------------------------
// constructor
mgColor::mgColor(
  DWORD rgb)
{
  m_r = 0xFF & (rgb >> 16);
  m_g = 0xFF & (rgb >> 8);
  m_b = 0xFF & rgb;
}

//--------------------------------------------------------------
// constructor
mgColor::mgColor(
  const mgColor& other)
{
  m_r = other.m_r;
  m_g = other.m_g;
  m_b = other.m_b;
}
    
//--------------------------------------------------------------
// assignment
mgColor& mgColor::operator=(
  const mgColor& other)
{
  m_r = other.m_r;
  m_g = other.m_g;
  m_b = other.m_b;
  return *this;
}
  
//--------------------------------------------------------------
// return as RGB DWORD
DWORD mgColor::asRGB() const
{
  return (m_r << 16) | (m_g << 8) | m_b;
}
