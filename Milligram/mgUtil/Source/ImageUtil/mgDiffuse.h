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
#ifndef MGDIFFUSE_H
#define MGDIFFUSE_H

class mgDiffuse
{
  BYTE m_aRedIndex[256];              // color table index for reds 
  BYTE m_aRedColor[256];              // output color level for reds 
  BYTE m_aGrnIndex[256];              // color table index for greens
  BYTE m_aGrnColor[256];              // output color level for greens
  BYTE m_aBluIndex[256];              // color table index for blues 
  BYTE m_aBluColor[256];              // output color level for blues

  int m_width;                     // image width
  short *m_apThisErr[3];            // error terms for each color, this line
  short *m_apNextErr[3];            // error terms for each color, next line

  // create lookup table for output colors
  void initOutColors(
    int nStartColor,                // start index in color table
    int nLevelCount,                // number of levels
    int nLevelMult,                 // index multiplier
    BYTE* pIndex,                   // color table indexes
    BYTE* pColor);                  // actual output colors

public:
  mgDiffuse(
    int nStartColor,                // first color table index 
    int width);                    // image width
  ~mgDiffuse();

  // convert scanline to color table using error diffusion
  void ConvertLine(
    BYTE* pSource,                  // r, g, b tripes, 1 byte per
    BYTE* pTarget);                 // output line 
};

#endif
