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
#ifndef CHUNKOBJ_H
#define CHUNKOBJ_H

// a column of blobs is 32 by 32 by 128 (4 blobs)
const int CHUNK_SIZE = 32;
const int CHUNK_COLUMN = 4;  // height of a column

const int CHUNK_UNLOADED = 0;
const int CHUNK_INMEMORY = 1;
const int CHUNK_INDISPLAY = 2;
const int CHUNK_NEEDSUPDATE = 3;

class ChunkWorld;
class MinecraftChunk;

#include "Bricks/BrickBlob.h"

// a chunk object
class ChunkObj
{
public:
  int m_originX;
  int m_originZ;

  int m_status;                      
  BOOL m_active;                      // a thread has claimed work on this chunk
  BOOL m_hasUpdate;                   // new display buffers available
  BOOL m_requested;                   // true if on the request list

  mgPoint3 m_eyePt;                   // eye point when createUpdate scheduled

  BrickBlob* m_blobs[CHUNK_COLUMN];

  // constructor
  ChunkObj(
    int x,
    int z,
    int status);

  // destructor
  virtual ~ChunkObj();

  // load the chunk file contents to memory
  virtual void loadMemory(
    ChunkWorld* world);

  // unload the chunk from memory
  virtual void unloadMemory();

  // create display buffers
  virtual void createBuffers(
    const mgPoint3& eyePt);

  // delete display buffers
  virtual void deleteBuffers();

  // return true if needs to be rebuilt
  virtual BOOL needsUpdate(
    const mgPoint3& eyePt);

  // create updated display buffers
  virtual void createUpdate(
    const mgPoint3& eyePt);

  // use updated buffers
  virtual void useUpdate();

  // update animation 
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // render
  virtual void render(
    ChunkWorld* world);

  // render transparent portion of object
  virtual void renderTransparent(
    ChunkWorld* world);

  // return size in bytes of object in memory
  virtual int getSystemMemUsed();

  // return size in bytes of object in display
  virtual int getDisplayMemUsed();

  // return true if any brick in view frustum
  virtual BOOL withinFrustum();

  // reset the age of a chunk
  virtual void resetAge();

protected:
  double m_createTime;                        // when loaded into display
  double m_alpha;                             // opacity during create

  // copy and translate mincraft chunk blocks
  void copyMinecraftBlocks(
    ChunkWorld* world,
    MinecraftChunk& chunk,
    int sourceX,
    int sourceY,
    int sourceZ,
    BrickBlob* blob,
    int targetX,
    int targetY,
    int targetZ,
    int xlen,
    int ylen,
    int zlen);
};

#endif
