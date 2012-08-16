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
#ifndef MGGIF3_H
#define MGGIF3_H

#include "mgLZW.h"

class mgGIFFileWrite : public mgLZWEncode
{
public:
  FILE* m_outFile;               // file to write 

  // write a GIF file header
  BOOL writeHeader(
    int width,                   // width of image
    int height,                  // height of image
    int nDataBits = 8,            // number of data bits
    int nColorBits = 8);          // number of color bits

  // write a GIF color table
  BOOL writeColorTable(
    DWORD *pColors,               // color table
    int nColorCount,              // number of colors
    int nColorBits = 8);          // declared size of table

  // write a GIF image header
  BOOL writeImageHeader(
    int width,                   // width of image
    int height,                  // height of image
    int nXPosn = 0,               // x position
    int nYPosn = 0,               // y position
    int nDataBits = 8);           // number of data bits

  // write a GIF image scanline
  BOOL writeLine(
    BYTE* line,                  // line of data
    int len);                    // length of line

  // finish image
  BOOL writeImageEnd();

  // finish file
  BOOL writeTerm();

  // constructor
  mgGIFFileWrite();

  // destructor
  virtual ~mgGIFFileWrite();

  // write a byte of LZW data
  virtual void writeLZWByte(
    BYTE nByte);

  // write a byte to GIF file
  virtual void writeByte(
    BYTE nByte)
  { fputc(nByte, m_outFile); }

  // write a word in GIF format
  void writeWord(
    int nWord)
  {
    writeByte(nWord & 0xFF);
    writeByte((nWord >> 8) & 0xFF);
  }

private:
  BYTE m_buffer[256];            // a block of LZW data
  int m_outPosn;                 // position in buffer
};

/*
// construct a gif file from a bitmap
BOOL GIFWriteBitmap(
  COLORREF *pColors,          // color table to use 
  int nColorCount,            // number of colors 
  CBitmap &bitmap,            // bitmap to convert
  const char* fileName);    // file to write 
*/

#ifdef WIN32
// construct a gif file from an array
BOOL GIFWriteArray(
  const char* fileName,     // file to write 
  BITMAPINFO *pInfo,          // bitmap info header
  int width,                 // width of image 
  int height,                // height of image 
  int nLineBytes,             // bytes per scanline
  BYTE* pData);               // data to write 
#endif

#endif
