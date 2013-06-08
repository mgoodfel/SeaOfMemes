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
#ifndef MGGENFONT_H
#define MGGENFONT_H

class mgGenSurface;
class mgGenContext;
class mgDisplayServices;

class mgGenFont : public mgFont
{
public:
  mgGenSurface* m_surface;
  int m_id;

  // constructor
  mgGenFont(
    mgGenSurface* surface,
    const char* faceName,
    int size,
    BOOL bold,
    BOOL italic);

  // destructor
  virtual ~mgGenFont();

  // measure a string
  virtual void stringExtent(
    const char* text,
    int textLen,
    mgPoint& endPt,
    mgRectangle& bounds) const;
    
  // measure a string (total advance)
  virtual int stringWidth(
    const char* text,
    int textLen) const;
    
  // return number of chars which fit in width
  virtual int stringFit(
    const char* text,
    int textLen,
    int width) const;
};

#endif
