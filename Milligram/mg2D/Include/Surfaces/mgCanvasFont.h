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
#ifndef MGCANVASFONT_H
#define MGCANVASFONT_H

extern "C"   // take off C++ decorated names
{
// initialize font handling
void mgCanvasInit();

// terminate font handling
void mgCanvasTerm();

// load a font and return handle
void* mgCanvasLoadFont(
  const char* faceName,
  double size,
  int dpi,
  BOOL bold,
  BOOL italic,
  double &fontHeight,
  double &fontAscent,
  double &charWidth);

// delete font
void mgCanvasDeleteFont(
  void* handle);

// get a character in the font
void mgCanvasGetChar(
  void* ftFace,
  int letter,
  double &advanceX,      // from start to end point
  double &advanceY,
  double &posnX,         // from start to top-left of image
  double &posnY,
  int &imageWidth,       // size of image
  int &imageHeight,
  BYTE*& imageData);
};

#endif
