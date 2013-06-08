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

#ifndef MINECRAFTCHUNK_H
#define MINECRAFTCHUNK_H

#include "MinecraftFile.h"

class MinecraftRegion;

// read minecraft chunk data from compressed region file
class MinecraftChunk : public MinecraftFile
{
public:
  // constructor
  MinecraftChunk();

  // destructor
  virtual ~MinecraftChunk();

  // parse a chunk from the region file
  virtual void parseRegion(
    MinecraftRegion* region,
    int chunkPosn,
    int chunkLen);

protected:
  void* m_zlibStream;               // zlib stream
  BYTE* m_zlibData;                 // compressed data

  MinecraftRegion* m_region;        // region file 
  int m_chunkPosn;                  // current position in region file of data
  int m_chunkLen;                   // length of data

  // read a buffer from the source file
  virtual void readSource();
};

#endif
