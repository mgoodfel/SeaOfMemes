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
#ifndef MGIMAGEUTIL_H
#define MGIMAGEUTIL_H

#pragma comment(lib, "JpegLib.lib")

class mgImageWrite;

// save BGRA image data to a jpg file
void mgWriteBGRAtoJPGFile(
  const char* fileName,
  int width,
  int height,
  const BYTE* pData);

// save BGR image data to a jpg file
void mgWriteBGRtoJPGFile(
  const char* fileName,
  int width,
  int height,
  const BYTE* pData);

// load an image file into memory
void mgLoadImage(
  const char* fileName,
  int &imageWidth,
  int &imageHeight,
  BYTE* &imageData);

// load image into RGBA format
void mgLoadRGBAImage(
  const char* fileName,
  int& width,
  int& height,
  BOOL& hasAlpha,
  BYTE*& data);

// read the image file
void mgReadImage(
  mgImageWrite* writer,
  const char* fileName);

#endif
