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
#ifndef MGTEXTDRAW_H
#define MGTEXTDRAW_H

#include "TextFormatter/mgTextFormat.h"

// format and draw text 
class mgTextDraw : public mgTextFormat
{
public:
  // constructor
  mgTextDraw(
    mgTextBuffer* buffer,         // text to format
    mgTextPage* page,             // formatting target
    mgRectangle& clip,            // clipping rectangle
    int originX,                  // origin of output
    int originY, 
    int pageWidth);               // target page width

  // destructor
  virtual ~mgTextDraw();

  // position a word
  virtual void outputLine(
    int left,                     // x position 
    int top,                      // y position
    int height,                   // height of line
    int baseline,                 // baseline of text
    mgPtrArray& frags);           // of mgFragDesc*

protected:
  mgRectangle m_clip;             // clipping rectangle
  int m_originX;                  // output origin
  int m_originY;
};

#endif

