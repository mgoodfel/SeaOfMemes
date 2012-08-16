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
#ifndef MINECRAFTREGION_H
#define MINECRAFTREGION_H

const int REGION_SIZE = 32;

class MinecraftRegion
{
public:
  // constructor
  MinecraftRegion(
    const char* regionDir,
    int regionX,
    int regionZ);

  // destructor
  virtual ~MinecraftRegion();

  // return file handle (open if necessary)
  virtual FILE* getFile();

  // close file handle
  virtual void closeFile();

  // get location of chunk in region file.  return false if not present
  virtual BOOL getChunkLoc(
    int chunkX,
    int chunkZ,
    int& chunkPosn, 
    int& chunkLen, 
    int& compressCode);

  // lock access for file i/o
  void lock()
  {
    m_lock->lock();
  }

  // unlock access
  void unlock()
  {
    m_lock->unlock();
  }
protected:
  // header is 4-byte locations for 32 by 32 chunks
  BYTE m_header[4*REGION_SIZE*REGION_SIZE];
  mgString m_fileName;
  FILE* m_regionFile;
  BOOL m_exists;

  // access lock
  mgOSLock* m_lock;
};

#endif
