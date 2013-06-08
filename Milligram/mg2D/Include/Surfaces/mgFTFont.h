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
#ifndef MGFTFONT_H
#define MGFTFONT_H

#pragma comment(lib, "FreeType.lib")

// initialize font handling
void mgFTInit();

// terminate font handling
void mgFTTerm();

// load a font and return handle
void* mgFTLoadFont(
  const char* fontFile,
  double size,
  int dpi,
  double &fontHeight,
  double &fontAscent,
  double &charWidth);

// delete font
void mgFTDeleteFont(
  void* handle);

// get a character in the font
void mgFTGetChar(
  void* ftFace,
  int letter,
  double &advanceX,      // from start to end point
  double &advanceY,
  double &posnX,         // from start to top-left of image
  double &posnY,
  int &imageWidth,       // size of image
  int &imageHeight,
  BYTE*& imageData);

#endif
