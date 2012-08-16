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

#include "BitmapSurface.h"

extern BOOL sysFindFont(
  const char* faceName, 
  BOOL bold, 
  BOOL italic, 
  mgString& fontFile);

//--------------------------------------------------------------
// constructor
BitmapSurface::BitmapSurface()
{
  m_bitmapDC = CreateCompatibleDC(NULL);
  SetGraphicsMode(m_bitmapDC, GM_ADVANCED);

  m_bitmapWidth = 0;
  m_bitmapHeight = 0;
  m_bitmapData = NULL;
  m_bitmap = NULL;
}

//--------------------------------------------------------------
// destructor
BitmapSurface::~BitmapSurface()
{
  if (m_bitmap != NULL)
    deleteBitmap();

  if (m_bitmapDC != NULL)
  {
    ReleaseDC(NULL, m_bitmapDC);
    m_bitmapDC = NULL;
  }
}

//--------------------------------------------------------------------
// repair damage
void BitmapSurface::repair(
  mgRectangle& bounds)
{
  if (isDamaged())
  {
    mgGenSurface::repair(bounds);

    // copy damaged rectangle into bitmap
    int copyWidth = bounds.m_width;
    int copyHeight = bounds.m_height;

    // copy the damaged area
    for (int y = 0; y < copyHeight; y++)
    {
      DWORD* bitmapLine = ((DWORD*) m_bitmapData) + m_imageWidth*(bounds.m_y+y) + bounds.m_x;
      DWORD* imageLine = m_imageData + m_imageWidth*(bounds.m_y+y) + bounds.m_x;
      for (int x = 0; x < copyWidth; x++)
      {
        // stupid Windows AlphaBlend routine wants me to pre-multiply my alpha values.
        DWORD value = imageLine[x];
        int alpha = value >> 24;
        int r = (value >> 16) & 0xFF;
        r = (alpha*r)/255;
        int g = (value >> 8) & 0xFF;
        g = (alpha*g)/255;
        int b = value & 0xFF;
        b = (alpha*b)/255;
        value = (alpha << 24) | (r << 16) | (g << 8) | b;
        bitmapLine[x] = value;
      }
      bitmapLine += m_imageWidth;
    }
  }
}

//--------------------------------------------------------------
// set dimensions of surface
void BitmapSurface::setSurfaceSize(
  int width,
  int height)
{
  if (width == m_bitmapWidth && height == m_bitmapHeight)
    return;

  mgGenSurface::setSurfaceSize(width, height);
  deleteBitmap();
  createBitmap(width, height);
  m_bitmapWidth = width;
  m_bitmapHeight = height;
}

//--------------------------------------------------------------
// return vertical pixels per inch
int BitmapSurface::displayDPI() const
{
  int dpi = GetDeviceCaps(m_bitmapDC, LOGPIXELSY);
  return max(dpi, 72);  // in case monitor info wrong
}

//--------------------------------------------------------------
// find a font file
BOOL BitmapSurface::findFont(
  const char* faceName, 
  BOOL bold, 
  BOOL italic, 
  mgString& fontFile)
{
  return sysFindFont(faceName, bold, italic, fontFile);
}

//--------------------------------------------------------------
// create a bitmap to hold the image
BOOL BitmapSurface::createBitmap(
  int width,
  int height)
{
  // create bitmap info header 
  BITMAPINFO *pInfo = (BITMAPINFO *) new char[sizeof(BITMAPINFOHEADER)];

  pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pInfo->bmiHeader.biPlanes = 1;
  pInfo->bmiHeader.biBitCount = 32;
  pInfo->bmiHeader.biCompression = BI_RGB;
  pInfo->bmiHeader.biSizeImage = 0;
  pInfo->bmiHeader.biClrUsed = 0;
  pInfo->bmiHeader.biClrImportant = 0;

  pInfo->bmiHeader.biWidth = width;
  pInfo->bmiHeader.biHeight = -height;

  void* pData;
  m_bitmap = CreateDIBSection(m_bitmapDC, pInfo, DIB_PAL_COLORS, &pData, NULL, 0);
  m_bitmapData = (BYTE*) pData;
  SelectObject(m_bitmapDC, m_bitmap);
  delete pInfo;

  // clear the bitmap 
  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = width;
  rect.bottom = height;
  FillRect(m_bitmapDC, &rect, (HBRUSH) GetStockObject(BLACK_BRUSH));

  return true;
}

//--------------------------------------------------------------
// delete screen bitmap
void BitmapSurface::deleteBitmap()
{
  SelectObject(m_bitmapDC, (HBITMAP) NULL);
  DeleteObject(m_bitmap);
  m_bitmap = NULL;
  m_bitmapData = NULL;
}

