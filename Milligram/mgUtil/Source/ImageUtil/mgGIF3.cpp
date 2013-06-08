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

#include "mgGIF3.h"

//-----------------------------------------------------------------
// constructor
mgGIFFileWrite::mgGIFFileWrite()
{
  m_outPosn = 0;
}

//-----------------------------------------------------------------
// destructor
mgGIFFileWrite::~mgGIFFileWrite()
{
}

//-----------------------------------------------------------------
// write a byte of LZW data
void mgGIFFileWrite::writeLZWByte(
  BYTE nByte)
{
  m_buffer[m_outPosn++] = nByte;
  if (m_outPosn == 255)
  {
    writeByte((BYTE) m_outPosn);
    fwrite(m_buffer, 1, m_outPosn, m_outFile);
    m_outPosn = 0;
  }
}

//-----------------------------------------------------------------
// write a GIF file header
BOOL mgGIFFileWrite::writeHeader(
  int width,                   // width of image
  int height,                  // height of image
  int nDataBits,                // number of data bits
  int nColorBits)               // number of color bits
{
  // write the image file header 
  fwrite("GIF87a", 1, 6, m_outFile);
  writeWord(width);      // screen width 
  writeWord(height);     // screen height 
  BYTE bFlags = 0x80;     // global color table
  bFlags |= (nDataBits-1) << 4;
  bFlags |= nColorBits-1;
  writeByte(bFlags);      
  writeByte(0x00);        // black background 
  writeByte(0);           // 1:1 aspect ratio 

  return TRUE;
}

//-----------------------------------------------------------------
// write a GIF color table
BOOL mgGIFFileWrite::writeColorTable(
  DWORD *pColors,               // color table
  int nColorCount,              // number of colors
  int nColorBits)               // number of color bits
{
  // write the global color table 
  int i = 0;
  for (; i < nColorCount; i++)
  {
    DWORD lColor = pColors[i];
    writeByte((BYTE) (lColor >> 16));  // red
    writeByte((BYTE) (lColor >> 8));   // green 
    writeByte((BYTE) lColor);        // blue
  }
  int nMaxColors = 1 << nColorBits;
  // pad to max colors
  for (; i < nMaxColors; i++)
  {
    writeByte(0);
    writeByte(0);
    writeByte(0);
  }

  return TRUE;
}

//-----------------------------------------------------------------
// write a GIF image header
BOOL mgGIFFileWrite::writeImageHeader(
  int width,                   // width of image
  int height,                  // height of image
  int nXPosn,                   // x position
  int nYPosn,                   // y position
  int nDataBits)                // number of data bits
{
  // write the image header 
  writeByte(0x2C);         // image introducer 
  writeWord(nXPosn);   // x position of image 
  writeWord(nYPosn);   // y position of image 
  writeWord(width);  // width of image 
  writeWord(height);  // height of image 

  writeByte(0x00);  // no local color table 

  // start lzw
  m_outPosn = 0;
  compressInit();
  writeByte((BYTE) nDataBits); // initial code size

  return TRUE;
}

//-----------------------------------------------------------------
// write a GIF image scanline
BOOL mgGIFFileWrite::writeLine(
  BYTE* line,                  // line of data
  int len)                     // length of line
{
  for (int i = 0; i < len; i++)
    compressByte(*line++);

  return TRUE;
}

//-----------------------------------------------------------------
// finish image
BOOL mgGIFFileWrite::writeImageEnd()
{
  compressTerm();

  // write the last block of image data
  if (m_outPosn > 0)
  {
    writeByte((BYTE) m_outPosn);
    fwrite(m_buffer, 1, m_outPosn, m_outFile);
  }

  writeByte(0x00);   // null block 

  return TRUE;
}

//-----------------------------------------------------------------
// finish file
BOOL mgGIFFileWrite::writeTerm()
{
  // write the trailer to end the file 
  writeByte(0x3B);

  return TRUE;
}

/*
//-----------------------------------------------------------------
// construct a gif file from a bitmap
BOOL GIFWriteBitmap(
  COLORREF *pColors,          // color table to use 
  int nColorCount,            // number of colors 
  CBitmap &bitmap,            // bitmap to convert
  const mgString& fileName)     // file to write 
{
  mgGIFFileWrite writer;

  // open the image file 
  writer.m_outFile = mgOSFileOpen(fileName, "wb");
  if (writer.m_outFile == NULL)
    return FALSE;

  // extract the image data 
  BITMAP bminfo;
  bitmap.GetObject(sizeof(bminfo), &bminfo);

  int nLineBytes = 2*((bminfo.bmWidth +1)/2);
  long lSize = nLineBytes * bminfo.bmHeight;
  BYTE* pData = new BYTE[lSize];
  long lActualSize = bitmap.GetBitmapBits(lSize, pData);

  // write the header
  writer.writeHeader(bminfo.bmWidth, bminfo.bmHeight);

  for (int i = 0; i < nColorCount; i++) 
  {
    writer.writeByte(GetRValue(pColors[i]));
    writer.writeByte(GetGValue(pColors[i]));
    writer.writeByte(GetBValue(pColors[i]));
  }
  for (; i < 256; i++)
  {
    writer.writeByte(0);
    writer.writeByte(0);
    writer.writeByte(0);
  }

  // write the image header
  writer.writeImageHeader(bminfo.bmWidth, bminfo.bmHeight);

  // write the image data 
  BYTE* line = pData;
  for (i = 0; i < bminfo.bmHeight; i++) 
  {
    writer.writeLine(line, bminfo.bmWidth);
    line += nLineBytes; // scanline width
  }
  delete pData;

  // complete data, close file
  writer.writeImageEnd();
  writer.writeTerm();
  fclose(writer.m_outFile);

  return TRUE;
}
*/

#ifdef WIN32
//-----------------------------------------------------------------
// construct a gif file from an array
BOOL GIFWriteArray(
  const mgString& fileName,     // file to write 
  BITMAPINFO *pInfo,            // bitmap info header
  int width,                    // width of image 
  int height,                   // height of image 
  int nLineBytes,               // bytes per scanline
  BYTE* pData)                  // data to write 
{
  mgGIFFileWrite writer;

  // open the image file 
  writer.m_outFile = mgOSFileOpen(fileName, "wb");
  if (writer.m_outFile == NULL)
    return FALSE;

  // write the header
  writer.writeHeader(width, height);

  // write the color table
  for (int i = 0; i < 256; i++) 
  {
    writer.writeByte(pInfo->bmiColors[i].rgbRed);
    writer.writeByte(pInfo->bmiColors[i].rgbGreen);
    writer.writeByte(pInfo->bmiColors[i].rgbBlue);
  }

  // write the image header
  writer.writeImageHeader(width, height);

  // write the image data 
  for (int i = 0; i < height; i++) 
  {
    writer.writeLine(pData, width);
    pData += nLineBytes; // scanline width
  }

  // complete data, close file
  writer.writeImageEnd();
  writer.writeTerm();
  fclose(writer.m_outFile);

  return TRUE;
}

#endif
