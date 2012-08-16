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

#include "mgXPImage.h"
#include "ImageUtil/mgImageUtil.h"

//--------------------------------------------------------------
// constructor
mgXPImage::mgXPImage(
  HDC imageDC,
  const mgString& fileName)
{
  BYTE* imageData;
  mgLoadRGBAImage(fileName, m_width, m_height, m_hasAlpha, imageData);

  // create bitmap info header 
  BITMAPINFO *info = (BITMAPINFO *) new char[sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD)];

  info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  info->bmiHeader.biPlanes = 1;
  info->bmiHeader.biBitCount = 24;
  info->bmiHeader.biCompression = BI_RGB;
  info->bmiHeader.biSizeImage = 0;
  info->bmiHeader.biClrUsed = 0;
  info->bmiHeader.biClrImportant = 0;

  info->bmiHeader.biWidth = m_width;
  info->bmiHeader.biHeight = -m_height;

  // copy RGB data into bitmap
  void* bitmapData;
  m_platformRGB = CreateDIBSection(imageDC, info, DIB_PAL_COLORS, &bitmapData, NULL, 0);

  int sourceRowSpan = 4*m_width;

  BYTE* target = (BYTE*) bitmapData;
  int targetRowSpan = 4*((m_width*3+3)/4);
  for (int row = 0; row < m_height; row++)
  {
    int sourcePosn = row*sourceRowSpan;
    int targetPosn = row*targetRowSpan;
    for (int col = 0; col < m_width; col++)
    {
      target[targetPosn+2] = imageData[sourcePosn+0];  // red
      target[targetPosn+1] = imageData[sourcePosn+1];  // green
      target[targetPosn+0] = imageData[sourcePosn+2];  // blue
      targetPosn += 3;
      sourcePosn += 4;
    }
  }

  // copy Alpha data into bitmap
  info->bmiHeader.biBitCount = 8;
  // create the grayscale color map
  RGBQUAD* colors = (RGBQUAD*) &info->bmiColors;
  for (int i = 0; i < 256; i++)
  {
    colors[i].rgbRed = (BYTE) i;
    colors[i].rgbGreen = (BYTE) i;
    colors[i].rgbBlue = (BYTE) i;
    colors[i].rgbReserved = (BYTE) 0;
  }
  m_platformAlpha = CreateDIBSection(imageDC, info, DIB_RGB_COLORS, &bitmapData, NULL, 0);

  target = (BYTE*) bitmapData;
  targetRowSpan = 4*((m_width+3)/4);
  for (int row = 0; row < m_height; row++)
  {
    int sourcePosn = row*sourceRowSpan;
    int targetPosn = row*targetRowSpan;
    for (int col = 0; col < m_width; col++)
    {
      target[targetPosn] = imageData[sourcePosn+3];  // alpha
      targetPosn++;
      sourcePosn += 4;
    }
  }

  delete info;
  delete imageData;
}

//--------------------------------------------------------------
// destructor
mgXPImage::~mgXPImage()
{
  if (m_platformRGB != NULL)
  {
    DeleteObject(m_platformRGB);
    m_platformRGB = NULL;
  }
  if (m_platformAlpha != NULL)
  {
    DeleteObject(m_platformAlpha);
    m_platformAlpha = NULL;
  }
}

#endif
