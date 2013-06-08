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

#include "mgDiffuse.h"

// Floyd-Steinberg error diffusion to convert color images to lower number
// of levels. See Graphics Gems II, section II.3

//------------------------------------------------------------------------
// constructor
mgDiffuse::mgDiffuse(
  int nStartColor,                // first color table index 
  int width)                     // width of image 
{
  m_width = width;

  // set output color tables based on number of levels
  initOutColors(nStartColor, 6, 6*6, m_aRedIndex, m_aRedColor);
  initOutColors(0, 6, 6, m_aGrnIndex, m_aGrnColor);
  initOutColors(0, 6, 1, m_aBluIndex, m_aBluColor);

  // allocate thisLine and nextLine error buffers
  // make them +2 length, so error terms can spill over
  for (int i = 0; i < 3; i++) 
  {
    m_apThisErr[i] = new short[m_width + 2];
    memset(m_apThisErr[i], 0, sizeof(short) * (m_width+2));
    m_apNextErr[i] = new short[m_width + 2];
    memset(m_apNextErr[i], 0, sizeof(short) * (m_width+2));
  }
}                     

//------------------------------------------------------------------------
// destructor 
mgDiffuse::~mgDiffuse()
{
  // free error buffers
  for (int i = 0; i < 3; i++) 
  {
    delete m_apThisErr[i];
    delete m_apNextErr[i];
  }
}

//-----------------------------------------------------------------------
// create lookup table for output colors
void mgDiffuse::initOutColors(
  int nStartColor,                      // start index in color table
  int nLevelCount,                      // number of levels
  int nLevelMult,                       // index multiplier
  BYTE* pIndex,                         // color table indexes
  BYTE* pColor)                         // actual output colors
{
  for (int i = 0; i < 256; i++) 
  {
    pIndex[i] = (BYTE) ((i * (nLevelCount - 1) + 128) / 255);
    pColor[i] = (BYTE) ((255 * pIndex[i]) / (nLevelCount-1));
    pIndex[i] = (BYTE) (nStartColor + pIndex[i] * nLevelMult);
  }
}

//------------------------------------------------------------------------
// convert scanline to color table using error diffusion
void mgDiffuse::ConvertLine(
  BYTE* pSource,                  // r, g, b tripes, 1 byte per
  BYTE* pTarget)                  // output line 
{
  short *pThisErr;                  // this error line
  short *pNextErr;                  // next error line
  short e3, e5, e7;                    // error terms

  // error buffers are plus 1 index, to eliminate test for zero 

  // do reds 
  pThisErr = m_apThisErr[0];
  pNextErr = m_apNextErr[0];

  int si = 0;       // source index, pixels are r, g, b
  for (int i = 0; i < m_width; i++, si+= 3) 
  {
    short nInColor = pSource[si] + pThisErr[i+1];
    short nInIndex = min(255, max(0, nInColor));
    pTarget[i] = m_aRedIndex[nInIndex];
    short nErr = nInColor - m_aRedColor[nInIndex];
    pNextErr[i] += e3 = (3*nErr)/16;
    pNextErr[i+1] += e5 = (5*nErr)/16;
    pThisErr[i+2] += e7 = (7*nErr)/16;
    pNextErr[i+2] += nErr-(e3+e5+e7);
  }

  // nextline becomes thisline 
  memcpy(pThisErr, pNextErr, sizeof(short) * m_width);
  memset(pNextErr, 0, sizeof(short) * m_width);

  // do greens
  pThisErr = m_apThisErr[1];
  pNextErr = m_apNextErr[1];

  si = 1;       // source index, pixels are r, g, b
  for (int i = 0; i < m_width; i++, si+= 3) 
  {
    short nInColor = pSource[si] + pThisErr[i+1];
    short nInIndex = min(255, max(0, nInColor));
    pTarget[i] += m_aGrnIndex[nInIndex];
    short nErr = nInColor - m_aGrnColor[nInIndex];
    pNextErr[i] += e3 = (3*nErr)/16;
    pNextErr[i+1] += e5 = (5*nErr)/16;
    pThisErr[i+2] += e7 = (7*nErr)/16;
    pNextErr[i+2] += nErr-(e3+e5+e7);
  }

  // nextline becomes thisline 
  memcpy(pThisErr, pNextErr, sizeof(short) * m_width);
  memset(pNextErr, 0, sizeof(short) * m_width);
  
  // do blues
  pThisErr = m_apThisErr[2];
  pNextErr = m_apNextErr[2];

  si = 2;       // source index, pixels are r, g, b
  for (int i = 0; i < m_width; i++, si+= 3) 
  {
    short nInColor = pSource[si] + pThisErr[i+1];
    short nInIndex = min(255, max(0, nInColor));
    pTarget[i] += m_aBluIndex[nInIndex];
    short nErr = nInColor - m_aBluColor[nInIndex];
    pNextErr[i] += e3 = (3*nErr)/16;
    pNextErr[i+1] += e5 = (5*nErr)/16;
    pThisErr[i+2] += e7 = (7*nErr)/16;
    pNextErr[i+2] += nErr-(e3+e5+e7);
  }

  // nextline becomes thisline 
  memcpy(pThisErr, pNextErr, sizeof(short) * m_width);
  memset(pNextErr, 0, sizeof(short) * m_width);
}

