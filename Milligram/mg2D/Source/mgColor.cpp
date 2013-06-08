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

#include "mgColor.h"

//--------------------------------------------------------------
// constructor
mgColor::mgColor(
  const char* spec)
{
  // unknown color is red
  m_r = 1.0; m_g = 0.0; m_b = 0.0; m_a = 1.0;

  if (_stricmp(spec, "black") == 0)
  {  m_r = 0.0; m_g = 0.0; m_b = 0.0; }

  else if (_stricmp(spec, "white") == 0)
  {  m_r = 1.0; m_g = 1.0; m_b = 1.0; }

  else if (_stricmp(spec, "lightGray") == 0)
  {  m_r = 0.75; m_g = 0.75; m_b = 0.75; }

  else if (_stricmp(spec, "lightGrey") == 0)
  {  m_r = 0.75; m_g = 0.75; m_b = 0.75; }

  else if (_stricmp(spec, "gray") == 0)
  {  m_r = 0.75; m_g = 0.75; m_b = 0.75; }

  else if (_stricmp(spec, "grey") == 0)
  {  m_r = 0.75; m_g = 0.75; m_b = 0.75; }

  else if (_stricmp(spec, "darkGray") == 0)
  {  m_r = 0.5; m_g = 0.5; m_b = 0.5; }

  else if (_stricmp(spec, "darkGrey") == 0)
  {  m_r = 0.5; m_g = 0.5; m_b = 0.5; }

  else if (_stricmp(spec, "red") == 0)
  {  m_r = 1.0; m_g = 0.0; m_b = 0.0; }

  else if (_stricmp(spec, "green") == 0)
  {  m_r = 0.0; m_g = 1.0; m_b = 0.0; }

  else if (_stricmp(spec, "blue") == 0)
  {  m_r = 0.0; m_g = 0.0; m_b = 1.0; }

  else if (_stricmp(spec, "yellow") == 0)
  {  m_r = 1.0; m_g = 1.0; m_b = 0.0; }

  else if (_stricmp(spec, "cyan") == 0)
  {  m_r = 1.0; m_g = 0.0; m_b = 1.0; }

  else if (_stricmp(spec, "magenta") == 0)
  {  m_r = 0.0; m_g = 1.0; m_b = 1.0; }

  else if (spec[0] == '#')
  {
    // find start of hex string
    int start = 1;
    while (spec[start] != '\0' && !isxdigit(spec[start]))
      start++;
    
    int value;
    if (1 == sscanf(spec+start, "%x", &value))
    {
      // find end of hex string
      int end = start;
      while (spec[end] != '\0' && isxdigit(spec[end]))
        end++;

      // if alpha specified (more than RRGGBB) 
      if (end-start > 6)
      {
        int ir = (value >> 24) & 0xFF;
        int ig = (value >> 16) & 0xFF;
        int ib = (value >> 8) & 0xFF;
        int ia = value & 0xFF;

        m_r = ir/255.0;
        m_g = ig/255.0;
        m_b = ib/255.0;
        m_a = ia/255.0;
      }
      else
      {
        int ir = (value >> 16) & 0xFF;
        int ig = (value >> 8) & 0xFF;
        int ib = value & 0xFF;

        m_r = ir/255.0;
        m_g = ig/255.0;
        m_b = ib/255.0;
        m_a = 1.0;
      }
    }
  }
}

//--------------------------------------------------------------
// constructor
mgColor::mgColor()
{
  m_r = m_g = m_b = 0.0;
  m_a = 1.0;
}
  
//--------------------------------------------------------------
// constructor
mgColor::mgColor(
  double r,
  double g,
  double b,
  double a)
{
  m_r = r;
  m_g = g;
  m_b = b;
  m_a = a;
}

//--------------------------------------------------------------
// constructor
mgColor::mgColor(
  DWORD rgb)
{
  // must match asRGB below
  int ir = 0xFF & (rgb >> 24);
  int ig = 0xFF & (rgb >> 16);
  int ib = 0xFF & (rgb >> 8);
  int ia = 0xFF & rgb;

  m_r = ir/255.0;
  m_g = ig/255.0;
  m_b = ib/255.0;
  m_a = ia/255.0;
}

//--------------------------------------------------------------
// constructor
mgColor::mgColor(
  const mgColor& other)
{
  m_r = other.m_r;
  m_g = other.m_g;
  m_b = other.m_b;
  m_a = other.m_a;
}
    
//--------------------------------------------------------------
// assignment
mgColor& mgColor::operator=(
  const mgColor& other)
{
  m_r = other.m_r;
  m_g = other.m_g;
  m_b = other.m_b;
  m_a = other.m_a;
  return *this;
}
  
//--------------------------------------------------------------
// true if colors equal
BOOL mgColor::equals(
  const mgColor& other)
{
  return m_r == other.m_r && 
         m_g == other.m_g && 
         m_b == other.m_b && 
         m_a == other.m_a; 
}

//--------------------------------------------------------------
// return color RGB as dword
DWORD mgColor::asRGB() const
{
  // must match constructor above
  int r = (int) (0.5+m_r*255);
  int g = (int) (0.5+m_g*255);
  int b = (int) (0.5+m_b*255);
  int a = (int) (0.5+m_a*255);
  return (r << 24) | (g << 16) | (b << 8) | a;
}
