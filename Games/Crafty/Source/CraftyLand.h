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

#ifndef CRAFTYLAND_H
#define CRAFTYLAND_H

#include "Landscape/Land.h"
#include "Landscape/CraftyTerrain.h"

class CraftyLand : public Land
{
public:
  // constructor
  CraftyLand(
    int threadCount),
  : Land(threadCount);

  // destructor
  virtual ~CraftyLand()
  {}

  // create new terrain subclass
  virtual Terrain* newTerrain()
  {
    return new CraftyTerrain();
  }
};

#endif

