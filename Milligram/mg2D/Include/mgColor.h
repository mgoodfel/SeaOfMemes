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
#ifndef MGCOLOR_H
#define MGCOLOR_H

/*
  A color, specified as RGB.  These are not managed resources and
  can be deleted.
*/
class mgColor 
{
public:
  double m_r;
  double m_g;
  double m_b;
  double m_a;

  // constructor
  mgColor();

  // constructor
  mgColor(
    const char* spec);
  
  // constructor
  mgColor(
    DWORD rgb);

  // constructor
  mgColor(
    double r,
    double g,
    double b,
    double a = 1.0);

  // constructor
  mgColor(
    const mgColor& other);
    
  // assignment
  mgColor& operator=(
    const mgColor& other);

  // true if colors equal
  BOOL equals(
    const mgColor& other);

  DWORD asRGB() const;
};

#endif
