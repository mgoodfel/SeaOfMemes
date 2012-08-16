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
#ifndef MGFONT_H
#define MGFONT_H

#include "mgResource.h"

class mgFont : public mgResource
{
public:
  // destructor
  virtual ~mgFont()
  {}

  // return face name
  virtual void getFaceName(
    mgString& faceName) const = 0;
  
  // return size in points
  virtual int getSize() const = 0;
  
  // return true if bold
  virtual BOOL getBold() const = 0;
  
  // return true if italic
  virtual BOOL getItalic() const = 0;
  
  // return ascent in pixels
  virtual int getAscent() const = 0;
  
  // return line spacing in pixels
  virtual int getHeight() const = 0;
  
  // return a char width for sizing
  virtual int getAveCharWidth() const = 0;

  // measure a string
  virtual void stringExtent(
    const char* text,
    int textLen,
    mgPoint& endPt,
    mgRectangle& bounds) const = 0;
    
  // measure a string (total advance)
  virtual int stringWidth(
    const char* text,
    int textLen) const = 0;
    
  // return number of chars which fit in width
  virtual int stringFit(
    const char* text,
    int textLen,
    int width) const = 0;
};

#endif
