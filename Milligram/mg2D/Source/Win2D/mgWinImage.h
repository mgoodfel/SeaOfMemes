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
#ifndef MGXPIMAGE_H
#define MGXPIMAGE_H

#include "Graphics2D/mgTypes2D.h"

class mgXPContext;
class mgImageWrite;

class mgXPImage : public mgImage
{
public:
  // constructor
  mgXPImage(
    HDC imageDC,
    const mgString& fileName);
    
  // destructor
  virtual ~mgXPImage();
  
  // return width
  virtual int getWidth() const
  {
    return m_width;
  }
  
  // return height
  virtual int getHeight() const
  {
    return m_height;
  }
  
  // has alpha channel
  virtual BOOL hasAlpha() const
  {
    return m_hasAlpha;
  }
  
protected:
  HBITMAP m_platformRGB;
  HBITMAP m_platformAlpha;
  int m_width;
  int m_height;
  BOOL m_hasAlpha;

  friend class mgXPContext;
};

#endif
